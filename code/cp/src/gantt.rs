use crate::base::SchedulingEvent;
use std::collections::HashMap;

/// Represents a single execution segment in the Gantt chart
#[derive(Debug, Clone)]
pub struct GanttSegment {
    pub label: String,
    pub start_time: u32,
    pub end_time: u32,
}

/// Gantt chart representation
#[derive(Debug)]
pub struct GanttChart {
    segments: Vec<GanttSegment>,
    total_time: u32,
}

impl GanttChart {
    /// Create a new Gantt chart from scheduling events
    pub fn from_events(events: impl Iterator<Item = SchedulingEvent>) -> Self {
        let mut segments = Vec::new();
        let mut total_time = 0;

        for event in events {
            match event {
                SchedulingEvent::ProcessExecution {
                    process_id,
                    start_time,
                    end_time,
                    ..
                } => {
                    segments.push(GanttSegment {
                        label: format!("P{}", process_id),
                        start_time,
                        end_time,
                    });
                    total_time = total_time.max(end_time);
                }
                SchedulingEvent::CpuIdle { time } => {
                    total_time = total_time.max(time);
                }
            }
        }

        GanttChart {
            segments,
            total_time,
        }
    }

    /// Get all segments
    pub fn segments(&self) -> &[GanttSegment] {
        &self.segments
    }

    /// Get total time
    pub fn total_time(&self) -> u32 {
        self.total_time
    }

    /// Render the Gantt chart as ASCII art (vertical format)
    pub fn render(&self) -> String {
        if self.segments.is_empty() {
            return String::from("No processes executed\n");
        }

        let mut output = String::new();
        let max_time = self.total_time;

        // Group segments by label
        let mut process_map: HashMap<String, Vec<&GanttSegment>> = HashMap::new();
        for segment in &self.segments {
            process_map
                .entry(segment.label.clone())
                .or_insert_with(Vec::new)
                .push(segment);
        }

        // Get sorted list of labels
        let mut labels: Vec<String> = process_map.keys().cloned().collect();
        labels.sort();

        // Render time header
        output.push_str("    ");
        for t in 0..=max_time {
            output.push_str(&format!("{:<3}", t));
        }
        output.push('\n');

        // Render time markers
        output.push_str("    ");
        for _ in 0..=max_time {
            output.push_str("│  ");
        }
        output.push('\n');

        // Render each process
        for label in labels {
            output.push_str(&format!("{:<3} ", label));

            let segments = &process_map[&label];

            for t in 0..max_time {
                // Check if this time falls within any segment for this process
                let is_executing = segments
                    .iter()
                    .any(|seg| t >= seg.start_time && t < seg.end_time);

                // Check if previous time was also executing (to remove separator)
                let prev_executing = if t > 0 {
                    segments
                        .iter()
                        .any(|seg| (t - 1) >= seg.start_time && (t - 1) < seg.end_time)
                } else {
                    false
                };

                if is_executing {
                    if prev_executing {
                        // Continue solid block without separator
                        output.push_str("███");
                    } else {
                        // Start of new segment
                        output.push_str("│██");
                    }
                } else {
                    output.push_str("│  ");
                }
            }
            output.push_str("│\n");
        }

        output.push('\n');
        output
    }

    /// Render a compact horizontal Gantt chart
    pub fn render_compact(&self) -> String {
        if self.segments.is_empty() {
            return String::from("No processes executed\n");
        }

        let mut output = String::new();
        output.push_str("Gantt Chart:\n");
        output.push_str("┌");

        // Top border
        for segment in &self.segments {
            let width = (segment.end_time - segment.start_time) as usize;
            output.push_str(&"─".repeat(width.max(3)));
            output.push('┬');
        }
        output.pop(); // Remove last ┬
        output.push_str("┐\n│");

        // Process labels
        for segment in &self.segments {
            let width = (segment.end_time - segment.start_time) as usize;
            let padding = width.max(3);
            output.push_str(&format!("{:^width$}", segment.label, width = padding));
            output.push('│');
        }
        output.push('\n');

        // Bottom border
        output.push('└');
        for segment in &self.segments {
            let width = (segment.end_time - segment.start_time) as usize;
            output.push_str(&"─".repeat(width.max(3)));
            output.push('┴');
        }
        output.pop(); // Remove last ┴
        output.push_str("┘\n");

        // Time markers
        for segment in &self.segments {
            output.push_str(&format!("{:<3}", segment.start_time));
        }
        output.push_str(&format!("{}\n", self.total_time));

        output
    }

    /// Print timeline of executions
    pub fn print_timeline(&self) {
        println!("Execution Timeline:");
        for segment in &self.segments {
            println!(
                "  {}: [{}..{}] (duration: {})",
                segment.label,
                segment.start_time,
                segment.end_time,
                segment.end_time - segment.start_time
            );
        }
        println!("Total time: {}", self.total_time);
    }

    /// Calculate statistics per process
    pub fn calculate_statistics(&self) -> Vec<ProcessStats> {
        let mut stats_map: HashMap<String, ProcessStats> = HashMap::new();

        for segment in &self.segments {
            let stats = stats_map
                .entry(segment.label.clone())
                .or_insert(ProcessStats {
                    label: segment.label.clone(),
                    total_execution_time: 0,
                    first_start_time: segment.start_time,
                    last_end_time: segment.end_time,
                    execution_count: 0,
                });

            stats.total_execution_time += segment.end_time - segment.start_time;
            stats.first_start_time = stats.first_start_time.min(segment.start_time);
            stats.last_end_time = stats.last_end_time.max(segment.end_time);
            stats.execution_count += 1;
        }

        let mut result: Vec<ProcessStats> = stats_map.into_values().collect();
        result.sort_by(|a, b| a.label.cmp(&b.label));
        result
    }
}

/// Statistics for a single process
#[derive(Debug, Clone)]
pub struct ProcessStats {
    pub label: String,
    pub total_execution_time: u32,
    pub first_start_time: u32,
    pub last_end_time: u32,
    pub execution_count: u32,
}

impl ProcessStats {
    /// Calculate turnaround time (last end - first start)
    pub fn turnaround_time(&self) -> u32 {
        self.last_end_time - self.first_start_time
    }

    /// Calculate waiting time (turnaround - execution)
    pub fn waiting_time(&self) -> u32 {
        self.turnaround_time() - self.total_execution_time
    }

    /// Print statistics
    pub fn print(&self) {
        println!("Process {}:", self.label);
        println!("  Execution time: {}", self.total_execution_time);
        println!("  Turnaround time: {}", self.turnaround_time());
        println!("  Waiting time: {}", self.waiting_time());
        println!("  Number of executions: {}", self.execution_count);
    }
}

/// Builder for creating Gantt charts with custom options
pub struct GanttChartBuilder {
    events: Vec<SchedulingEvent>,
}

impl GanttChartBuilder {
    pub fn new() -> Self {
        GanttChartBuilder { events: Vec::new() }
    }

    pub fn add_event(mut self, event: SchedulingEvent) -> Self {
        self.events.push(event);
        self
    }

    pub fn add_events(mut self, events: impl Iterator<Item = SchedulingEvent>) -> Self {
        self.events.extend(events);
        self
    }

    pub fn build(self) -> GanttChart {
        GanttChart::from_events(self.events.into_iter())
    }
}

impl Default for GanttChartBuilder {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_gantt_chart_creation() {
        let events = vec![
            SchedulingEvent::ProcessExecution {
                process_id: 1,
                start_time: 0,
                end_time: 4,
                is_completed: false,
            },
            SchedulingEvent::ProcessExecution {
                process_id: 2,
                start_time: 4,
                end_time: 8,
                is_completed: false,
            },
        ];

        let chart = GanttChart::from_events(events.into_iter());
        assert_eq!(chart.segments().len(), 2);
        assert_eq!(chart.total_time(), 8);
        assert_eq!(chart.segments()[0].label, "P1");
        assert_eq!(chart.segments()[1].label, "P2");
    }

    #[test]
    fn test_statistics_calculation() {
        let events = vec![
            SchedulingEvent::ProcessExecution {
                process_id: 1,
                start_time: 0,
                end_time: 4,
                is_completed: false,
            },
            SchedulingEvent::ProcessExecution {
                process_id: 1,
                start_time: 8,
                end_time: 10,
                is_completed: true,
            },
        ];

        let chart = GanttChart::from_events(events.into_iter());
        let stats = chart.calculate_statistics();

        assert_eq!(stats.len(), 1);
        assert_eq!(stats[0].label, "P1");
        assert_eq!(stats[0].total_execution_time, 6);
        assert_eq!(stats[0].turnaround_time(), 10);
        assert_eq!(stats[0].waiting_time(), 4);
    }

    #[test]
    fn test_empty_chart() {
        let chart = GanttChart::from_events(std::iter::empty());
        assert_eq!(chart.segments().len(), 0);
        assert_eq!(chart.total_time(), 0);
    }

    #[test]
    fn test_render_non_empty() {
        let events = vec![SchedulingEvent::ProcessExecution {
            process_id: 1,
            start_time: 0,
            end_time: 2,
            is_completed: true,
        }];

        let chart = GanttChart::from_events(events.into_iter());
        let output = chart.render();
        assert!(output.contains("P1"));
        assert!(!output.contains("No processes"));
    }
}
