use crate::gantt::{GanttChart, GanttSegment};
use plotters::prelude::*;
use std::collections::HashMap;

const COLORS: &[(u8, u8, u8)] = &[
    (52, 152, 219),
    (46, 204, 113),
    (155, 89, 182),
    (230, 126, 34),
    (231, 76, 60),
    (26, 188, 156),
    (241, 196, 15),
    (149, 165, 166),
    (236, 240, 241),
    (52, 73, 94),
];

pub struct GanttVisualizer<'a> {
    chart: &'a GanttChart,
    title: String,
    width: u32,
    height: u32,
}

impl<'a> GanttVisualizer<'a> {
    pub fn new(chart: &'a GanttChart) -> Self {
        GanttVisualizer {
            chart,
            title: "Round Robin Scheduling - Gantt Chart".to_string(),
            width: 1200,
            height: 600,
        }
    }

    pub fn with_title(mut self, title: impl Into<String>) -> Self {
        self.title = title.into();
        self
    }

    pub fn with_size(mut self, width: u32, height: u32) -> Self {
        self.width = width;
        self.height = height;
        self
    }

    pub fn save_png(&self, path: &str) -> Result<(), Box<dyn std::error::Error>> {
        let root = BitMapBackend::new(path, (self.width, self.height)).into_drawing_area();
        self.draw(root)?;
        Ok(())
    }

    pub fn save_svg(&self, path: &str) -> Result<(), Box<dyn std::error::Error>> {
        let root = SVGBackend::new(path, (self.width, self.height)).into_drawing_area();
        self.draw(root)?;
        Ok(())
    }

    fn draw<DB: DrawingBackend>(
        &self,
        root: DrawingArea<DB, plotters::coord::Shift>,
    ) -> Result<(), Box<dyn std::error::Error>>
    where
        DB::ErrorType: 'static,
    {
        root.fill(&WHITE)?;

        let segments = self.chart.segments();
        if segments.is_empty() {
            root.titled("No processes to display", ("sans-serif", 40))?;
            root.present()?;
            return Ok(());
        }

        let mut process_segments: HashMap<String, Vec<&GanttSegment>> = HashMap::new();
        for segment in segments {
            process_segments
                .entry(segment.label.clone())
                .or_insert_with(Vec::new)
                .push(segment);
        }

        let mut process_labels: Vec<String> = process_segments.keys().cloned().collect();
        process_labels.sort();

        let total_time = self.chart.total_time();
        let num_processes = process_labels.len();

        let margin = 80;
        let chart_area = root.titled(&self.title, ("sans-serif", 40).into_font())?;

        let mut chart = ChartBuilder::on(&chart_area)
            .margin(margin)
            .x_label_area_size(40)
            .y_label_area_size(80)
            .build_cartesian_2d(0f32..(total_time as f32), 0f32..(num_processes as f32))?;

        chart
            .configure_mesh()
            .disable_y_mesh()
            .x_desc("Time")
            .y_desc("Process")
            .y_labels(num_processes)
            .y_label_formatter(&|y| {
                let idx = *y as usize;
                if idx < process_labels.len() {
                    process_labels[idx].clone()
                } else {
                    String::new()
                }
            })
            .draw()?;

        for (process_idx, process_label) in process_labels.iter().enumerate() {
            let segments = &process_segments[process_label];
            let color = self.get_color_for_process(process_idx);

            for segment in segments {
                let y = process_idx as f32;
                let x1 = segment.start_time as f32;
                let x2 = segment.end_time as f32;

                chart.draw_series(std::iter::once(Rectangle::new(
                    [(x1, y + 0.1), (x2, y + 0.9)],
                    color.filled(),
                )))?;

                chart.draw_series(std::iter::once(Rectangle::new(
                    [(x1, y + 0.1), (x2, y + 0.9)],
                    BLACK.mix(0.8),
                )))?;

                let width = x2 - x1;
                if width > 2.0 {
                    let mid_x = (x1 + x2) / 2.0;
                    let mid_y = y + 0.5;
                    let label = format!("{}-{}", segment.start_time, segment.end_time);
                    chart.draw_series(std::iter::once(Text::new(
                        label,
                        (mid_x, mid_y),
                        ("sans-serif", 12).into_font().color(&BLACK),
                    )))?;
                }
            }
        }

        root.present()?;
        Ok(())
    }

    fn get_color_for_process(&self, index: usize) -> RGBColor {
        let (r, g, b) = COLORS[index % COLORS.len()];
        RGBColor(r, g, b)
    }
}

pub fn save_gantt_png(chart: &GanttChart, path: &str) -> Result<(), Box<dyn std::error::Error>> {
    GanttVisualizer::new(chart).save_png(path)
}

pub fn save_gantt_svg(chart: &GanttChart, path: &str) -> Result<(), Box<dyn std::error::Error>> {
    GanttVisualizer::new(chart).save_svg(path)
}

pub fn save_statistics_chart(
    chart: &GanttChart,
    path: &str,
) -> Result<(), Box<dyn std::error::Error>> {
    let stats = chart.calculate_statistics();
    if stats.is_empty() {
        return Ok(());
    }

    let root = BitMapBackend::new(path, (1000, 600)).into_drawing_area();
    root.fill(&WHITE)?;

    let root = root.titled("Process Statistics", ("sans-serif", 40).into_font())?;

    let max_time = stats
        .iter()
        .map(|s| s.turnaround_time().max(s.total_execution_time))
        .max()
        .unwrap_or(1);

    let process_labels: Vec<String> = stats.iter().map(|s| s.label.clone()).collect();

    let mut chart = ChartBuilder::on(&root)
        .margin(20)
        .x_label_area_size(40)
        .y_label_area_size(80)
        .build_cartesian_2d(
            0f32..(process_labels.len() as f32),
            0f32..(max_time as f32 * 1.1),
        )?;

    chart
        .configure_mesh()
        .y_desc("Time Units")
        .x_desc("Process")
        .x_labels(process_labels.len())
        .x_label_formatter(&|x| {
            let idx = *x as usize;
            if idx < process_labels.len() {
                process_labels[idx].clone()
            } else {
                String::new()
            }
        })
        .draw()?;

    let bar_width = 0.25;
    for (i, stat) in stats.iter().enumerate() {
        let x = i as f32;

        chart.draw_series(std::iter::once(Rectangle::new(
            [(x - bar_width, 0.0), (x, stat.total_execution_time as f32)],
            BLUE.mix(0.7).filled(),
        )))?;

        chart.draw_series(std::iter::once(Rectangle::new(
            [(x, 0.0), (x + bar_width, stat.waiting_time() as f32)],
            RED.mix(0.7).filled(),
        )))?;
    }

    chart
        .draw_series(std::iter::once(Rectangle::new(
            [(0.5, max_time as f32 * 0.95), (1.0, max_time as f32 * 0.98)],
            BLUE.mix(0.7).filled(),
        )))?
        .label("Execution Time")
        .legend(|(x, y)| Rectangle::new([(x, y - 5), (x + 10, y + 5)], BLUE.mix(0.7).filled()));

    chart
        .draw_series(std::iter::once(Rectangle::new(
            [(0.5, max_time as f32 * 0.90), (1.0, max_time as f32 * 0.93)],
            RED.mix(0.7).filled(),
        )))?
        .label("Waiting Time")
        .legend(|(x, y)| Rectangle::new([(x, y - 5), (x + 10, y + 5)], RED.mix(0.7).filled()));

    chart
        .configure_series_labels()
        .background_style(&WHITE.mix(0.8))
        .border_style(&BLACK)
        .draw()?;

    root.present()?;
    Ok(())
}
