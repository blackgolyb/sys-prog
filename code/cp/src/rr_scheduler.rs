use std::cell::RefCell;
use std::collections::VecDeque;
use std::rc::Rc;

use crate::base::{Process, Scheduler, SchedulingEvent};

/// Iterator that yields scheduling events
pub struct RRScheduleIterator {
    time_quantum: u32,
    ready_queue: VecDeque<Process>,
    current_time: u32,
    scheduler: Rc<RefCell<RRScheduler>>,
    current_execution: Option<(Process, u32)>, // (process, start_time)
}

impl RRScheduleIterator {
    fn new(time_quantum: u32, scheduler: Rc<RefCell<RRScheduler>>) -> Self {
        Self {
            time_quantum,
            ready_queue: VecDeque::new(),
            current_time: 0,
            scheduler,
            current_execution: None,
        }
    }

    fn check_for_new_processes(&mut self) {
        let mut scheduler = self.scheduler.borrow_mut();
        // Move all new processes from scheduler to ready queue
        while let Some(process) = scheduler.processes.pop() {
            self.ready_queue.push_back(process);
        }
    }
}

impl Iterator for RRScheduleIterator {
    type Item = SchedulingEvent;

    fn next(&mut self) -> Option<Self::Item> {
        // Check for new processes added to scheduler
        self.check_for_new_processes();

        // If we have a current execution, continue or finish it
        if let Some((mut process, start_time)) = self.current_execution.take() {
            let execution_time = self.current_time - start_time;

            if execution_time >= self.time_quantum || process.is_completed() {
                // Time quantum expired or process completed
                let end_time = self.current_time;
                let is_completed = process.is_completed();
                let process_id = process.id;

                // If not completed, put back in ready queue
                if !is_completed {
                    process.pause();
                    self.ready_queue.push_back(process);
                }

                self.current_time = end_time;

                return Some(SchedulingEvent::ProcessExecution {
                    process_id,
                    start_time,
                    end_time,
                    is_completed,
                });
            } else {
                // Continue execution
                process.perform();
                self.current_time += 1;
                self.current_execution = Some((process, start_time));
                return self.next();
            }
        }

        // No current execution, check for new processes again
        self.check_for_new_processes();

        // Try to get next process from ready queue
        if let Some(mut process) = self.ready_queue.pop_front() {
            let start_time = self.current_time;
            process.perform();
            self.current_time += 1;
            self.current_execution = Some((process, start_time));
            return self.next();
        }

        // No processes available, return None to end iteration
        None
    }
}

pub struct RRScheduler {
    time_quantum: u32,
    processes: Vec<Process>,
    scheduler_ref: Option<Rc<RefCell<RRScheduler>>>,
}

impl RRScheduler {
    pub fn new(time_quantum: u32) -> Rc<RefCell<Self>> {
        let scheduler = Rc::new(RefCell::new(RRScheduler {
            time_quantum,
            processes: Vec::new(),
            scheduler_ref: None,
        }));

        scheduler.borrow_mut().scheduler_ref = Some(Rc::clone(&scheduler));
        scheduler
    }

    pub fn get_time_quantum(&self) -> u32 {
        self.time_quantum
    }

    pub fn reset(&mut self) {
        self.processes.clear();
    }

    /// Execute scheduling and return iterator
    pub fn schedule(&mut self) -> RRScheduleIterator {
        RRScheduleIterator::new(
            self.time_quantum,
            Rc::clone(self.scheduler_ref.as_ref().unwrap()),
        )
    }
}

impl Scheduler for RRScheduler {
    fn add_process(&mut self, process: Process) {
        self.processes.push(process);
    }

    fn schedule(&mut self) -> Box<dyn Iterator<Item = SchedulingEvent>> {
        Box::new(RRScheduleIterator::new(
            self.time_quantum,
            Rc::clone(self.scheduler_ref.as_ref().unwrap()),
        ))
    }
}

#[test]
fn test_empty_scheduler() {
    let scheduler = RRScheduler::new(4);
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
    let scheduler = RRScheduler::new(10);
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
