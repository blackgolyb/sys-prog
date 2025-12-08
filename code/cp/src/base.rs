use std::fmt;

#[derive(Debug, Clone, PartialEq)]
pub enum ProcessState {
    Idel,
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
            state: ProcessState::Idel,
        }
    }

    pub fn perform(&mut self) {
        if self.remaining_time > 0 {
            self.remaining_time -= 1;
            self.state = ProcessState::Running;
        } else {
            self.state = ProcessState::Completed;
        }
    }

    pub fn pause(&mut self) {
        if self.state == ProcessState::Running {
            self.state = ProcessState::Idel;
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
