use std::fmt;

#[derive(Debug, Clone, PartialEq)]
pub enum ProcessState {
    Idle,
    Running,
    Completed,
}

#[derive(Debug, Clone)]
pub struct Process {
    pub id: u32,
    burst_time: u32,
    remaining_time: u32,
    state: ProcessState,
}

impl Process {
    pub fn new(id: u32, burst_time: u32) -> Self {
        Process {
            id,
            burst_time,
            remaining_time: burst_time,
            state: ProcessState::Idle,
        }
    }

    pub fn perform(&mut self, time_quantum: u32) -> u32 {
        let remaining_time = self.remaining_time;
        if remaining_time > 0 {
            self.remaining_time = self.remaining_time.saturating_sub(time_quantum);
            self.state = ProcessState::Running;
            remaining_time - self.remaining_time
        } else {
            self.state = ProcessState::Completed;
            0
        }
    }

    pub fn pause(&mut self) {
        if self.state == ProcessState::Running {
            self.state = ProcessState::Idle;
        }
    }

    pub fn is_completed(&self) -> bool {
        self.state == ProcessState::Completed
    }
}

/// Events that can occur during scheduling
#[derive(Debug, Clone)]
pub enum SchedulingEvent {
    CpuIdle {
        time: u32,
    },
    ProcessExecution {
        process_id: u32,
        start_time: u32,
        end_time: u32,
        is_completed: bool,
    },
}

pub trait Scheduler {
    fn schedule(&mut self) -> Box<dyn Iterator<Item = SchedulingEvent>>;
    fn add_process(&mut self, process: Process);
}

#[test]
fn test_process_perform() {
    struct Case {
        remaining: u32,
        quantum: u32,
        new_remaining: u32,
        processed: u32,
    }

    #[rustfmt::skip]
    let cases = [
        Case { remaining: 3, quantum: 7,  new_remaining: 0, processed: 3 },
        Case { remaining: 4, quantum: 3,  new_remaining: 1, processed: 3 },
        Case { remaining: 1, quantum: 1,  new_remaining: 0, processed: 1 },
        Case { remaining: 0, quantum: 10, new_remaining: 0, processed: 0 },
        Case { remaining: 0, quantum: 0,  new_remaining: 0, processed: 0 },
        Case { remaining: 3, quantum: 0,  new_remaining: 3, processed: 0 },
    ];

    for case in cases {
        let mut process = Process::new(1, case.remaining);
        let processed = process.perform(case.quantum);

        assert_eq!(
            process.remaining_time, case.new_remaining,
            "remaining_time mismatch for case: remaining={}, quantum={}",
            case.remaining, case.quantum
        );

        assert_eq!(
            processed, case.processed,
            "processed mismatch for case: remaining={}, quantum={}",
            case.remaining, case.quantum
        );
    }
}
