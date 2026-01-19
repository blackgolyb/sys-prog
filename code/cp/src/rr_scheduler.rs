use std::cell::RefCell;
use std::rc::Rc;

use crate::base::{Process, Scheduler, SchedulingEvent};

/// Iterator that yields scheduling events
pub struct RRScheduleIterator<F>
where
    F: Fn(u32) -> u32 + 'static,
{
    current_time: u32,
    idle_time: u32,
    stop_after_idle_time: u32,
    scheduler: Rc<RefCell<RRScheduler<F>>>,
}

impl<F> RRScheduleIterator<F>
where
    F: Fn(u32) -> u32 + 'static,
{
    fn new(scheduler: Rc<RefCell<RRScheduler<F>>>) -> Self {
        Self {
            current_time: 0,
            idle_time: 0,
            stop_after_idle_time: 10,
            scheduler,
        }
    }

    fn get_process(&mut self) -> Option<Process> {
        let mut scheduler = self.scheduler.borrow_mut();
        scheduler.processes.pop()
    }

    fn put_back_process(&mut self, process: Process) {
        let mut scheduler = self.scheduler.borrow_mut();
        scheduler.processes.push(process);
    }

    fn time_quantum(&self) -> u32 {
        let scheduler = self.scheduler.borrow();
        scheduler.get_time_quantum()
    }
}

impl<F> Iterator for RRScheduleIterator<F>
where
    F: Fn(u32) -> u32 + 'static,
{
    type Item = SchedulingEvent;

    fn next(&mut self) -> Option<Self::Item> {
        let time_quantum = self.time_quantum();
        if let Some(mut process) = self.get_process() {
            self.idle_time = 0;
            let processed_time = process.perform(time_quantum);

            let start_time = self.current_time;
            let id = process.id;
            let is_completed = process.is_completed();
            self.current_time += processed_time;

            if !is_completed {
                process.pause();
                self.put_back_process(process);
            }

            return Some(SchedulingEvent::ProcessExecution {
                process_id: id,
                start_time,
                end_time: self.current_time,
                is_completed,
            });
        } else if self.idle_time < self.stop_after_idle_time {
            let idle_start = self.current_time;
            self.current_time += 1;
            self.idle_time += 1;

            return Some(SchedulingEvent::CpuIdle { time: idle_start });
        }

        None
    }
}

pub struct RRScheduler<F>
where
    F: Fn(u32) -> u32 + 'static,
{
    time_quantum_func: F,
    processes: Vec<Process>,
    scheduler_ref: Option<Rc<RefCell<RRScheduler<F>>>>,
}

impl<F> RRScheduler<F>
where
    F: Fn(u32) -> u32 + 'static,
{
    pub fn new(time_quantum_func: F) -> Rc<RefCell<Self>> {
        let scheduler = Rc::new(RefCell::new(RRScheduler {
            time_quantum_func,
            processes: Vec::new(),
            scheduler_ref: None,
        }));

        scheduler.borrow_mut().scheduler_ref = Some(Rc::clone(&scheduler));
        scheduler
    }

    pub fn get_time_quantum(&self) -> u32 {
        (self.time_quantum_func)(self.processes.len() as u32)
    }

    pub fn reset(&mut self) {
        self.processes.clear();
    }

    pub fn schedule(&mut self) -> RRScheduleIterator<F> {
        RRScheduleIterator::new(Rc::clone(self.scheduler_ref.as_ref().unwrap()))
    }
}

impl<F> Scheduler for RRScheduler<F>
where
    F: Fn(u32) -> u32 + 'static,
{
    fn add_process(&mut self, process: Process) {
        self.processes.push(process);
    }

    fn schedule(&mut self) -> Box<dyn Iterator<Item = SchedulingEvent>> {
        Box::new(RRScheduleIterator::new(Rc::clone(
            self.scheduler_ref.as_ref().unwrap(),
        )))
    }
}

#[test]
fn test_empty_scheduler() {
    let scheduler = RRScheduler::new(|_| 4);
    let mut iter = scheduler.borrow_mut().schedule();
    let mut events = Vec::new();
    while let Some(event) = iter.next() {
        events.push(event);
    }
    assert_eq!(events.len(), 0);
}

#[test]
fn test_single_process_simple() {
    // Process with burst time less than quantum completes in one go
    let scheduler = RRScheduler::new(|_| 10);
    scheduler.borrow_mut().add_process(Process::new(1, 5));

    let mut iter = scheduler.borrow_mut().schedule();
    let mut events = Vec::new();
    while let Some(event) = iter.next() {
        events.push(event);
    }

    assert_eq!(events.len(), 1);
    match &events[0] {
        SchedulingEvent::ProcessExecution {
            process_id,
            start_time,
            end_time,
            is_completed,
        } => {
            assert_eq!(*process_id, 1);
            assert_eq!(*start_time, 0);
            // Process may complete at any time up to burst_time depending on scheduler
            assert!(*end_time >= 5);
            assert!(is_completed);
        }
        _ => panic!("Expected ProcessExecution event"),
    }
}
