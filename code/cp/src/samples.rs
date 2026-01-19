use crate::rr_scheduler::RRScheduler;
use crate::simulation::ScheduleSimulation;

macro_rules! sample {
    (
        $name:ident,
        description: $description:expr,
        time_quantum: $time_quantum:expr,
        processes: [
            $(
                $processes:expr,
            )*
        ]
    ) => {
        pub fn $name() -> ScheduleSimulation<impl Fn(u32) -> u32> {
            let processes = vec![
                $(
                    // Arrival Time, Burst Time
                    $processes
                ),*
            ];
            let time_quantum_func = $time_quantum;
            let scheduler = RRScheduler::new(time_quantum_func);
            ScheduleSimulation::new(processes, scheduler, $description.to_owned())
        }
    };
}

fn _clamp<T>(min: T, max: T, num: T) -> T
where
    T: Ord,
{
    if num < min {
        min
    } else if num > max {
        max
    } else {
        num
    }
}

fn clamp(min: u32, max: u32, value: u32) -> u32 {
    match (min, max) {
        (0, 0) => value.max(1),
        (_, 0) => value.max(min),
        (0, _) => _clamp(1, max, value),
        _ => _clamp(min, max, value),
    }
}

fn linear(offset: f64, factor: f64) -> impl Fn(u32) -> u32 {
    move |x: u32| ((x as f64) * factor + offset).round().max(1.0) as u32
}

fn polynomial(offset: f64, degree: f64) -> impl Fn(u32) -> u32 {
    move |x: u32| ((x as f64).powf(degree) + offset).round().max(1.0) as u32
}

fn logarithmic(offset: f64, base: f64) -> impl Fn(u32) -> u32 {
    move |x: u32| {
        let log_value = if x > 0 { (x as f64).log(base) } else { 0.0 };
        (log_value + offset).round().max(1.0) as u32
    }
}

fn bounded_scaling(min: u32, max: u32, function: impl Fn(u32) -> u32) -> impl Fn(u32) -> u32 {
    move |size: u32| clamp(min, max, function(size))
}

fn fixed_time_quantum(quantum: u32) -> impl Fn(u32) -> u32 {
    move |_| quantum
}

// Public time quantum presets for TUI
pub fn time_quantum_fixed_small() -> impl Fn(u32) -> u32 {
    fixed_time_quantum(2)
}

pub fn time_quantum_fixed_medium() -> impl Fn(u32) -> u32 {
    fixed_time_quantum(4)
}

pub fn time_quantum_fixed_large() -> impl Fn(u32) -> u32 {
    fixed_time_quantum(8)
}

pub fn time_quantum_linear_bounded() -> impl Fn(u32) -> u32 {
    bounded_scaling(2, 8, linear(2.0, 1.0))
}

pub fn time_quantum_linear_unbounded() -> impl Fn(u32) -> u32 {
    linear(1.0, 1.0)
}

pub fn time_quantum_polynomial() -> impl Fn(u32) -> u32 {
    bounded_scaling(2, 10, polynomial(1.0, 2.0))
}

pub fn time_quantum_logarithmic() -> impl Fn(u32) -> u32 {
    bounded_scaling(2, 8, logarithmic(2.0, 2.0))
}

pub fn get_time_quantum_preset_name(index: usize) -> &'static str {
    match index {
        0 => "Fixed Small (2)",
        1 => "Fixed Medium (4)",
        2 => "Fixed Large (8)",
        3 => "Linear Bounded (2-8)",
        4 => "Linear Unbounded",
        5 => "Polynomial (x² + 1, max 10)",
        6 => "Logarithmic (log₂(x) + 2)",
        _ => "Unknown",
    }
}

sample! {
    basic,
    description: "Basic Round Robin - 3 Processes",
    time_quantum: bounded_scaling(2, 8, linear(2.0, 1.0)),
    processes: [
        (0, 8),
        (0, 6),
        (0, 4),
    ]
}

sample! {
    staggered_arrivals,
    description: "Staggered Arrivals",
    time_quantum: bounded_scaling(2, 8, linear(2.0, 1.0)),
    processes: [
        (0, 10),
        (3, 5),
        (6, 8),
    ]
}

sample! {
    complex_scenario,
    description: "Complex Scenario - 6 Processes",
    time_quantum: bounded_scaling(2, 8, linear(2.0, 1.0)),
    processes: [
        (0, 12),
        (0, 8),
        (2, 5),
        (5, 10),
        (8, 4),
        (10, 6),
    ]
}

sample! {
    high_load,
    description: "High Load - 5 Processes",
    time_quantum: bounded_scaling(2, 8, linear(2.0, 1.0)),
    processes: [
        (0, 15),
        (2, 8),
        (4, 5),
        (6, 12),
        (8, 7),
    ]
}
