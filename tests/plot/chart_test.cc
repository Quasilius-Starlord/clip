/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "plot/plot_layout.h"
#include "plot/axisdefinition.h"
#include "plot/domain.h"
#include "plot/linechart.h"
#include "plot/series.h"
#include "utils/fileutil.h"
#include "utils/stringutil.h"
#include "../unittest.h"

UNIT_TEST(ChartTest);

using plotfx::chart::AnyDomain;
using plotfx::chart::AxisDefinition;
//using plotfx::chart::AreaChart;
//using plotfx::chart::AreaChart2D;
//using plotfx::chart::AreaChart3D;
//using plotfx::chart::BarChart;
//using plotfx::chart::BarChart2D;
//using plotfx::chart::BarChart3D;
using plotfx::chart::LineChart;
//using plotfx::chart::PointChart2D;
//using plotfx::chart::PointChart3D;
using plotfx::Color;
using plotfx::Viewport;
using namespace plotfx::chart;
using plotfx::chart::ContinuousDomain;
//using plotfx::chart::DomainProvider;
using plotfx::chart::Series;
using plotfx::chart::Series2D;
using plotfx::chart::Series3D;
using plotfx::FileOutputStream;
using plotfx::FileUtil;
using plotfx::StringUtil;
using plotfx::Layer;
using plotfx::test::UnitTest;

//static void compareChart(
//    PlotLayout* chart,
//    const std::string& file_name) {
//  auto output_path = file_name;
//  StringUtil::replaceAll(&output_path, ".svg", ".actual.svg");
//
//  auto output_stream = FileOutputStream::openFile(output_path);
//
//
//  EXPECT_FILES_EQ(file_name, output_path);
//}

TEST_CASE(ChartTest, TestPlotLayoutWithLeftAxis, [] () {
  Layer target(1000, 600);
  target.clear(Color{1, 1, 1, 1});

  //auto axis_left = plot_layout.addAxis(AxisDefinition::LEFT);
  //axis_left->addTick(0.0);
  //axis_left->addTick(0.2);
  //axis_left->addTick(0.4);
  //axis_left->addTick(0.6);
  //axis_left->addTick(0.8);
  //axis_left->addTick(1.0);
  //axis_left->addLabel(0.0, "0");
  //axis_left->addLabel(0.2, "1");
  //axis_left->addLabel(0.4, "2");
  //axis_left->addLabel(0.6, "3");
  //axis_left->addLabel(0.8, "4");
  //axis_left->addLabel(1.0, "5");

  //plot_layout.render(&target);
  {
    GridDefinition grid(GridDefinition::GRID_HORIZONTAL);
    grid.addTick(0.0);
    grid.addTick(0.2);
    grid.addTick(0.4);
    grid.addTick(0.6);
    grid.addTick(0.8);
    grid.addTick(1.0);
    renderGrid(grid, Viewport{target.width, target.height}, &target);
  }

  {
    GridDefinition grid(GridDefinition::GRID_VERTICAL);
    grid.addTick(0.0);
    grid.addTick(0.2);
    grid.addTick(0.4);
    grid.addTick(0.6);
    grid.addTick(0.8);
    grid.addTick(1.0);
    renderGrid(grid, Viewport{target.width, target.height}, &target);
  }

  target.writePNG("test_plot_layout_with_left_axis.png");
});

//TEST_CASE(ChartTest, TestPlotLayoutWithLeftAxisAndTitle, [] () {
//  PlotLayout canvas;
//
//  auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
//  axis_left->setTitle("my axis");
//  axis_left->addTick(0.0);
//  axis_left->addTick(0.2);
//  axis_left->addTick(0.4);
//  axis_left->addTick(0.6);
//  axis_left->addTick(0.8);
//  axis_left->addTick(1.0);
//  axis_left->addLabel(0.0, "0");
//  axis_left->addLabel(0.2, "1");
//  axis_left->addLabel(0.4, "2");
//  axis_left->addLabel(0.6, "3");
//  axis_left->addLabel(0.8, "4");
//  axis_left->addLabel(1.0, "5");
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestPlotLayoutWithLeftAxisAndTitle_out.svg");
//});
//
//TEST_CASE(ChartTest, TestPlotLayoutWithLeftAndBottomAxis, [] () {
//  PlotLayout canvas;
//
//  auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
//  axis_left->setTitle("my axis");
//  axis_left->addTick(0.0);
//  axis_left->addTick(0.2);
//  axis_left->addTick(0.4);
//  axis_left->addTick(0.6);
//  axis_left->addTick(0.8);
//  axis_left->addTick(1.0);
//  axis_left->addLabel(0.0, "0");
//  axis_left->addLabel(0.2, "1");
//  axis_left->addLabel(0.4, "2");
//  axis_left->addLabel(0.6, "3");
//  axis_left->addLabel(0.8, "4");
//  axis_left->addLabel(1.0, "5");
//
//  auto axis_bottom = canvas.addAxis(AxisDefinition::BOTTOM);
//  axis_bottom->setTitle("my axis");
//  axis_bottom->addTick(0.0);
//  axis_bottom->addTick(0.2);
//  axis_bottom->addTick(0.4);
//  axis_bottom->addTick(0.6);
//  axis_bottom->addTick(0.8);
//  axis_bottom->addTick(1.0);
//  axis_bottom->addLabel(0.2, "A");
//  axis_bottom->addLabel(0.4, "B");
//  axis_bottom->addLabel(0.6, "C");
//  axis_bottom->addLabel(0.8, "D");
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestPlotLayoutWithLeftAndBottomAxis_out.svg");
//});
//
//TEST_CASE(ChartTest, TestPlotLayoutWithAllAxis, [] () {
//  PlotLayout canvas;
//
//  auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
//  axis_left->setTitle("my axis");
//  axis_left->addTick(0.0);
//  axis_left->addTick(0.2);
//  axis_left->addTick(0.4);
//  axis_left->addTick(0.6);
//  axis_left->addTick(0.8);
//  axis_left->addTick(1.0);
//  axis_left->addLabel(0.0, "0");
//  axis_left->addLabel(0.2, "1");
//  axis_left->addLabel(0.4, "2");
//  axis_left->addLabel(0.6, "3");
//  axis_left->addLabel(0.8, "4");
//  axis_left->addLabel(1.0, "5");
//
//  auto axis_right = canvas.addAxis(AxisDefinition::RIGHT);
//  axis_right->setTitle("my axis");
//  axis_right->addTick(0.0);
//  axis_right->addTick(0.2);
//  axis_right->addTick(0.4);
//  axis_right->addTick(0.6);
//  axis_right->addTick(0.8);
//  axis_right->addTick(1.0);
//  axis_right->addLabel(0.0, "0");
//  axis_right->addLabel(0.2, "1");
//  axis_right->addLabel(0.4, "2");
//  axis_right->addLabel(0.6, "3");
//  axis_right->addLabel(0.8, "4");
//  axis_right->addLabel(1.0, "5");
//
//  auto axis_bottom = canvas.addAxis(AxisDefinition::BOTTOM);
//  axis_bottom->setTitle("my axis");
//  axis_bottom->addTick(0.0);
//  axis_bottom->addTick(0.2);
//  axis_bottom->addTick(0.4);
//  axis_bottom->addTick(0.6);
//  axis_bottom->addTick(0.8);
//  axis_bottom->addTick(1.0);
//  axis_bottom->addLabel(0.2, "A");
//  axis_bottom->addLabel(0.4, "B");
//  axis_bottom->addLabel(0.6, "C");
//  axis_bottom->addLabel(0.8, "D");
//
//  auto axis_top = canvas.addAxis(AxisDefinition::TOP);
//  axis_top->setTitle("my axis");
//  axis_top->addTick(0.0);
//  axis_top->addTick(0.2);
//  axis_top->addTick(0.4);
//  axis_top->addTick(0.6);
//  axis_top->addTick(0.8);
//  axis_top->addTick(1.0);
//  axis_top->addLabel(0.2, "A");
//  axis_top->addLabel(0.4, "B");
//  axis_top->addLabel(0.6, "C");
//  axis_top->addLabel(0.8, "D");
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestPlotLayoutWithAllAxis_out.svg");
//});
//
//TEST_CASE(ChartTest, TestPlotLayoutWithAllMultiAxis, [] () {
//  PlotLayout canvas;
//
//  {
//    auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
//    axis_left->setTitle("my axis");
//    axis_left->addTick(0.0);
//    axis_left->addTick(0.2);
//    axis_left->addTick(0.4);
//    axis_left->addTick(0.6);
//    axis_left->addTick(0.8);
//    axis_left->addTick(1.0);
//    axis_left->addLabel(0.0, "0");
//    axis_left->addLabel(0.2, "1");
//    axis_left->addLabel(0.4, "2");
//    axis_left->addLabel(0.6, "3");
//    axis_left->addLabel(0.8, "4");
//    axis_left->addLabel(1.0, "5");
//  }
//
//  {
//    auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
//    axis_left->addTick(0.0);
//    axis_left->addTick(0.2);
//    axis_left->addTick(0.4);
//    axis_left->addTick(0.6);
//    axis_left->addTick(0.8);
//    axis_left->addTick(1.0);
//    axis_left->addLabel(0.0, "0");
//    axis_left->addLabel(0.2, "1");
//    axis_left->addLabel(0.4, "2");
//    axis_left->addLabel(0.6, "3");
//    axis_left->addLabel(0.8, "4");
//    axis_left->addLabel(1.0, "5");
//  }
//
//  {
//    auto axis_right = canvas.addAxis(AxisDefinition::RIGHT);
//    axis_right->setTitle("my axis");
//    axis_right->addTick(0.0);
//    axis_right->addTick(0.2);
//    axis_right->addTick(0.4);
//    axis_right->addTick(0.6);
//    axis_right->addTick(0.8);
//    axis_right->addTick(1.0);
//    axis_right->addLabel(0.0, "0");
//    axis_right->addLabel(0.2, "1");
//    axis_right->addLabel(0.4, "2");
//    axis_right->addLabel(0.6, "3");
//    axis_right->addLabel(0.8, "4");
//    axis_right->addLabel(1.0, "5");
//  }
//
//  {
//    auto axis_right = canvas.addAxis(AxisDefinition::RIGHT);
//    axis_right->addTick(0.0);
//    axis_right->addTick(0.2);
//    axis_right->addTick(0.4);
//    axis_right->addTick(0.6);
//    axis_right->addTick(0.8);
//    axis_right->addTick(1.0);
//    axis_right->addLabel(0.0, "0");
//    axis_right->addLabel(0.2, "1");
//    axis_right->addLabel(0.4, "2");
//    axis_right->addLabel(0.6, "3");
//    axis_right->addLabel(0.8, "4");
//    axis_right->addLabel(1.0, "5");
//  }
//
//  {
//    auto axis_bottom = canvas.addAxis(AxisDefinition::BOTTOM);
//    axis_bottom->setTitle("my axis");
//    axis_bottom->addTick(0.0);
//    axis_bottom->addTick(0.2);
//    axis_bottom->addTick(0.4);
//    axis_bottom->addTick(0.6);
//    axis_bottom->addTick(0.8);
//    axis_bottom->addTick(1.0);
//    axis_bottom->addLabel(0.2, "A");
//    axis_bottom->addLabel(0.4, "B");
//    axis_bottom->addLabel(0.6, "C");
//    axis_bottom->addLabel(0.8, "D");
//  }
//
//  {
//    auto axis_bottom = canvas.addAxis(AxisDefinition::BOTTOM);
//    axis_bottom->addTick(0.0);
//    axis_bottom->addTick(0.2);
//    axis_bottom->addTick(0.4);
//    axis_bottom->addTick(0.6);
//    axis_bottom->addTick(0.8);
//    axis_bottom->addTick(1.0);
//    axis_bottom->addLabel(0.2, "A");
//    axis_bottom->addLabel(0.4, "B");
//    axis_bottom->addLabel(0.6, "C");
//    axis_bottom->addLabel(0.8, "D");
//  }
//
//  {
//    auto axis_top = canvas.addAxis(AxisDefinition::TOP);
//    axis_top->setTitle("my axis");
//    axis_top->addTick(0.0);
//    axis_top->addTick(0.2);
//    axis_top->addTick(0.4);
//    axis_top->addTick(0.6);
//    axis_top->addTick(0.8);
//    axis_top->addTick(1.0);
//    axis_top->addLabel(0.2, "A");
//    axis_top->addLabel(0.4, "B");
//    axis_top->addLabel(0.6, "C");
//    axis_top->addLabel(0.8, "D");
//  }
//
//  {
//    auto axis_top = canvas.addAxis(AxisDefinition::TOP);
//    axis_top->addTick(0.0);
//    axis_top->addTick(0.2);
//    axis_top->addTick(0.4);
//    axis_top->addTick(0.6);
//    axis_top->addTick(0.8);
//    axis_top->addTick(1.0);
//    axis_top->addLabel(0.2, "A");
//    axis_top->addLabel(0.4, "B");
//    axis_top->addLabel(0.6, "C");
//    axis_top->addLabel(0.8, "D");
//  }
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestPlotLayoutWithAllMultiAxis_out.svg");
//});
//
//TEST_CASE(ChartTest, TestPlotLayoutWithMultiLeftAxis, [] () {
//  PlotLayout canvas;
//
//  auto axis1 = canvas.addAxis(AxisDefinition::LEFT);
//  axis1->setTitle("axis 1 / axis 2");
//  axis1->addTick(0.0);
//  axis1->addTick(0.2);
//  axis1->addTick(0.4);
//  axis1->addTick(0.6);
//  axis1->addTick(0.8);
//  axis1->addTick(1.0);
//  axis1->addLabel(0.0, "0");
//  axis1->addLabel(0.2, "1");
//  axis1->addLabel(0.4, "2");
//  axis1->addLabel(0.6, "3");
//  axis1->addLabel(0.8, "4");
//  axis1->addLabel(1.0, "5");
//
//  auto axis2 = canvas.addAxis(AxisDefinition::LEFT);
//  axis2->addTick(0.0);
//  axis2->addTick(0.3);
//  axis2->addTick(0.6);
//  axis2->addTick(0.9);
//  axis2->addTick(1.0);
//  axis2->addLabel(0.0, "A");
//  axis2->addLabel(0.3, "B");
//  axis2->addLabel(0.6, "C");
//  axis2->addLabel(0.9, "D");
//  axis2->addLabel(1.0, "E");
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestPlotLayoutWithMultiLeftAxis_out.svg");
//});
//
//TEST_CASE(ChartTest, TestPlotLayoutWithAxisFromNumericalDomain, [] () {
//  PlotLayout canvas;
//
//  ContinuousDomain<double> domain(0, 100);
//  auto axis1 = canvas.addAxis(AxisDefinition::LEFT);
//  DomainProvider domain_adapter(&domain);
//  axis1->setDomain(&domain_adapter);
//  axis1->setTitle("numerical domain");
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestPlotLayoutWithAxisFromNumericalDomain_out.svg");
//});
//
//
//static plotfx::test::UnitTest::TestCase __test_simple_bar_chart_(
//    &ChartTest, "TestSimpleBarChart", [] () {
//  auto series = new Series2D<std::string, double>("myseries");
//
//  series->addDatum("A", 34);
//  series->addDatum("B", 18);
//  series->addDatum("C", 43);
//  series->addDatum("D", 19);
//  series->addDatum("E", 25);
//  series->addDatum("F", 33);
//
//  PlotLayout canvas;
//  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
//      BarChart::O_VERTICAL);
//
//  bar_chart->addSeries(series);
//  bar_chart->addAxis(AxisDefinition::TOP);
//  bar_chart->addAxis(AxisDefinition::RIGHT);
//  bar_chart->addAxis(AxisDefinition::BOTTOM);
//  bar_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestSimpleBarChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_mulitseries_bar_chart_(
//    &ChartTest, "TestMultiSeriesBarChart", [] () {
//  auto series1 = new Series2D<std::string, double>("myseries1");
//  series1->addDatum("A", 40);
//  series1->addDatum("B", 35);
//  series1->addDatum("C", 30);
//  series1->addDatum("D", 25);
//  series1->addDatum("E", 20);
//  series1->addDatum("F", 15);
//
//  auto series2 = new Series2D<std::string, double>("myseries2");
//  series2->addDatum("A", 23);
//  series2->addDatum("B", 65);
//  series2->addDatum("C", 43);
//  series2->addDatum("D", 12);
//  series2->addDatum("E", 54);
//  series2->addDatum("F", 31);
//
//  PlotLayout canvas;
//  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
//      BarChart::O_VERTICAL);
//
//  bar_chart->addSeries(series1);
//  bar_chart->addSeries(series2);
//  bar_chart->addAxis(AxisDefinition::TOP);
//  bar_chart->addAxis(AxisDefinition::RIGHT);
//  bar_chart->addAxis(AxisDefinition::BOTTOM);
//  bar_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestMultiSeriesBarChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_stacked_bar_chart_(
//    &ChartTest, "TestStackedBarChart", [] () {
//  auto series1 = new Series2D<std::string, double>("myseries1");
//  series1->addDatum("A", 40);
//  series1->addDatum("B", 35);
//  series1->addDatum("C", 30);
//  series1->addDatum("D", 25);
//  series1->addDatum("E", 20);
//  series1->addDatum("F", 15);
//
//  auto series2 = new Series2D<std::string, double>("myseries2");
//  series2->addDatum("A", 23);
//  series2->addDatum("B", 65);
//  series2->addDatum("C", 43);
//  series2->addDatum("D", 12);
//  series2->addDatum("E", 54);
//  series2->addDatum("F", 31);
//
//  PlotLayout canvas;
//  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
//      BarChart::O_VERTICAL, true);
//
//  bar_chart->addSeries(series1);
//  bar_chart->addSeries(series2);
//  bar_chart->addAxis(AxisDefinition::TOP);
//  bar_chart->addAxis(AxisDefinition::RIGHT);
//  bar_chart->addAxis(AxisDefinition::BOTTOM);
//  bar_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestStackedBarChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_horizontal_bar_chart_(
//    &ChartTest, "TestHorizontalBarChart", [] () {
//  auto series1 = new Series2D<std::string, double>("myseries1");
//  series1->addDatum("A", 40);
//  series1->addDatum("B", 35);
//  series1->addDatum("C", 30);
//  series1->addDatum("D", 25);
//  series1->addDatum("E", 20);
//  series1->addDatum("F", 15);
//
//  PlotLayout canvas;
//  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
//      BarChart::O_HORIZONTAL);
//
//  bar_chart->addSeries(series1);
//  bar_chart->addAxis(AxisDefinition::TOP);
//  bar_chart->addAxis(AxisDefinition::RIGHT);
//  bar_chart->addAxis(AxisDefinition::BOTTOM);
//  bar_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestHorizontalBarChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_horizontal_mulit_bar_chart_(
//    &ChartTest, "TestHorizontalMulitSeriesBarChart", [] () {
//  auto series1 = new Series2D<std::string, double>("myseries1");
//  series1->addDatum("A", 40);
//  series1->addDatum("B", 35);
//  series1->addDatum("C", 30);
//  series1->addDatum("D", 25);
//  series1->addDatum("E", 20);
//  series1->addDatum("F", 15);
//
//  auto series2 = new Series2D<std::string, double>("myseries2");
//  series2->addDatum("A", 23);
//  series2->addDatum("B", 65);
//  series2->addDatum("C", 43);
//  series2->addDatum("D", 12);
//  series2->addDatum("E", 54);
//  series2->addDatum("F", 31);
//
//  PlotLayout canvas;
//  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
//      BarChart::O_HORIZONTAL);
//
//  bar_chart->addSeries(series1);
//  bar_chart->addSeries(series2);
//  bar_chart->addAxis(AxisDefinition::TOP);
//  bar_chart->addAxis(AxisDefinition::RIGHT);
//  bar_chart->addAxis(AxisDefinition::BOTTOM);
//  bar_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestHorizontalMulitSeriesBarChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_horiz_stacked_bar_chart_(
//    &ChartTest, "TestHorizontalStackedBarChart", [] () {
//  auto series1 = new Series2D<std::string, double>("myseries1");
//  series1->addDatum("A", 40);
//  series1->addDatum("B", 35);
//  series1->addDatum("C", 30);
//  series1->addDatum("D", 25);
//  series1->addDatum("E", 20);
//  series1->addDatum("F", 15);
//
//  auto series2 = new Series2D<std::string, double>("myseries2");
//  series2->addDatum("A", 23);
//  series2->addDatum("B", 65);
//  series2->addDatum("C", 43);
//  series2->addDatum("D", 12);
//  series2->addDatum("E", 54);
//  series2->addDatum("F", 31);
//
//  PlotLayout canvas;
//  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
//      BarChart::O_HORIZONTAL, true);
//  bar_chart->addSeries(series1);
//  bar_chart->addSeries(series2);
//  bar_chart->addAxis(AxisDefinition::TOP);
//  bar_chart->addAxis(AxisDefinition::RIGHT);
//  bar_chart->addAxis(AxisDefinition::BOTTOM);
//  bar_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestHorizontalStackedBarChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_range_bar_chart_(
//    &ChartTest, "TestRangeBarChart", [] () {
//  auto series1 = new Series3D<std::string, double, double>("myseries1");
//  series1->addDatum("A", -40, +40);
//  series1->addDatum("B", -35, +35);
//  series1->addDatum("C", -30, +30);
//  series1->addDatum("D", -25, +25);
//  series1->addDatum("E", -20, +20);
//  series1->addDatum("F", -15, +15);
//
//  auto series2 = new Series3D<std::string, double, double>("myseries2");
//  series2->addDatum("A", -23, +23);
//  series2->addDatum("B", -65, +65);
//  series2->addDatum("C", -43, +43);
//  series2->addDatum("D", -12, +12);
//  series2->addDatum("E", -54, +54);
//  series2->addDatum("F", -31, +31);
//
//  PlotLayout canvas;
//  auto bar_chart = canvas.addChart<BarChart3D<std::string, double, double>>(
//      BarChart::O_VERTICAL);
//  bar_chart->addSeries(series1);
//  bar_chart->addSeries(series2);
//  bar_chart->addAxis(AxisDefinition::TOP);
//  bar_chart->addAxis(AxisDefinition::RIGHT);
//  bar_chart->addAxis(AxisDefinition::BOTTOM);
//  bar_chart->addAxis(AxisDefinition::LEFT)->setTitle("myaxis");
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestRangeBarChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_horiz_range_bar_chart_(
//    &ChartTest, "TestHorizontalRangeBarChart", [] () {
//  auto series1 = new Series3D<std::string, double, double>("myseries1");
//  series1->addDatum("A", -40, +40);
//  series1->addDatum("B", -35, +35);
//  series1->addDatum("C", -30, +30);
//  series1->addDatum("D", -25, +25);
//  series1->addDatum("E", -20, +20);
//  series1->addDatum("F", -15, +15);
//
//  auto series2 = new Series3D<std::string, double, double>("myseries2");
//  series2->addDatum("A", -23, +23);
//  series2->addDatum("B", -65, +65);
//  series2->addDatum("C", -43, +43);
//  series2->addDatum("D", -12, +12);
//  series2->addDatum("E", -54, +54);
//  series2->addDatum("F", -31, +31);
//
//  PlotLayout canvas;
//  auto bar_chart = canvas.addChart<BarChart3D<std::string, double, double>>(
//      BarChart::O_HORIZONTAL);
//  bar_chart->addSeries(series1);
//  bar_chart->addSeries(series2);
//  bar_chart->addAxis(AxisDefinition::TOP);
//  bar_chart->addAxis(AxisDefinition::RIGHT);
//  bar_chart->addAxis(AxisDefinition::BOTTOM)->setTitle("myaxis");
//  bar_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestHorizontalRangeBarChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_simple_point_chart_(
//    &ChartTest, "TestSimplePointChart", [] () {
//  auto series1 = new Series2D<double, double>("myseries1");
//  series1->addDatum(10, 34);
//  series1->addDatum(15, -18);
//  series1->addDatum(20, 43);
//  series1->addDatum(30, -19);
//  series1->addDatum(40, 25);
//  series1->addDatum(50, 33);
//
//  auto series2 = new Series2D<double, double>("myseries1");
//  series2->addDatum(-10, 34);
//  series2->addDatum(-15, 18);
//  series2->addDatum(-20, -43);
//  series2->addDatum(-30, 19);
//  series2->addDatum(-40, -25);
//  series2->addDatum(-50, -33);
//
//  PlotLayout canvas;
//  auto point_chart = canvas.addChart<PointChart2D<double, double>>();
//  point_chart->addSeries(series1);
//  point_chart->addSeries(series2);
//  point_chart->addAxis(AxisDefinition::TOP);
//  point_chart->addAxis(AxisDefinition::RIGHT);
//  point_chart->addAxis(AxisDefinition::BOTTOM);
//  point_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestSimplePointChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_variablesize_point_chart_(
//    &ChartTest, "TestVariableSizePointChart", [] () {
//  auto series1 = new Series3D<double, double, double>("myseries1");
//  series1->addDatum(10, 34, 5);
//  series1->addDatum(15, -18, 23);
//  series1->addDatum(20, 43, 11);
//  series1->addDatum(30, -19, 9);
//  series1->addDatum(40, 25, 14);
//  series1->addDatum(50, 33, 8);
//
//  auto series2 = new Series3D<double, double, double>("myseries1");
//  series2->addDatum(-10, 34, 5);
//  series2->addDatum(-15, 18, 8);
//  series2->addDatum(-20, -4, 23);
//  series2->addDatum(-30, 19, 2);
//  series2->addDatum(-40, -25, 4);
//  series2->addDatum(-50, -23, 17);
//
//  PlotLayout canvas;
//  auto point_chart = canvas.addChart<PointChart3D<double, double, double>>();
//  point_chart->addSeries(series1);
//  point_chart->addSeries(series2);
//  point_chart->addAxis(AxisDefinition::TOP);
//  point_chart->addAxis(AxisDefinition::RIGHT);
//  point_chart->addAxis(AxisDefinition::BOTTOM);
//  point_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestVariableSizePointChart_out.svg");
//});
//
static plotfx::test::UnitTest::TestCase __test_simple_line_chart_(
    &ChartTest, "TestSimpleLineChart", [] () {
  Layer target(1000, 600);
  target.clear(Color{ 1, 1, 1, 1 });

  auto series1 = new Series2D<double, double>("myseries1");
  series1->addDatum(10, 34);
  series1->addDatum(15, 38);
  series1->addDatum(20, 43);
  series1->addDatum(30, 33);
  series1->addDatum(40, 21);
  series1->addDatum(50, 33);

  auto series2 = new Series2D<double, double>("myseries1");
  series2->addDatum(10, 19);
  series2->addDatum(15, 18);
  series2->addDatum(20, 22);
  series2->addDatum(30, 23);
  series2->addDatum(40, 18);
  series2->addDatum(50, 21);

  ContinuousDomain<double> x_domain(10, 50, false);
  ContinuousDomain<double> y_domain(0, 50, false);

  LineChart line_chart(&x_domain, &y_domain);
  line_chart.addSeries(series1);
  line_chart.addSeries(series2);

  Viewport vp(target.width, target.height);
  line_chart.render(&target, &vp);

  target.writePNG("test_simple_line_chart.png");
  //compareChart(
  //    &canvas,
  //    "ChartTest_TestSimpleLineChart_out.svg");
});

//static plotfx::test::UnitTest::TestCase __test_point_line_chart_(
//    &ChartTest, "TestPointLineChart", [] () {
//  auto series1 = new Series2D<double, double>("myseries1");
//  series1->addDatum(10, 34);
//  series1->addDatum(15, 38);
//  series1->addDatum(20, 43);
//  series1->addDatum(30, 33);
//  series1->addDatum(40, 21);
//  series1->addDatum(50, 33);
//  series1->setDefaultProperty(Series::P_POINT_STYLE, "circle");
//
//  auto series2 = new Series2D<double, double>("myseries1");
//  series2->addDatum(10, 19);
//  series2->addDatum(15, 18);
//  series2->addDatum(20, 22);
//  series2->addDatum(30, 23);
//  series2->addDatum(40, 18);
//  series2->addDatum(50, 21);
//  series2->setDefaultProperty(Series::P_POINT_STYLE, "circle");
//
//  ContinuousDomain<double> x_domain(10, 50, false);
//  ContinuousDomain<double> y_domain(0, 50, false);
//
//  PlotLayout canvas;
//  auto line_chart = canvas.addChart<LineChart2D<double, double>>(
//      &x_domain, &y_domain);
//
//  line_chart->addSeries(series1);
//  line_chart->addSeries(series2);
//  line_chart->addAxis(AxisDefinition::BOTTOM);
//  line_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestPointLineChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_multi_chart_(
//    &ChartTest, "TestMultiChart", [] () {
//  auto series1 = new Series2D<double, double>("myseries1");
//  series1->addDatum(10, 34);
//  series1->addDatum(15, 38);
//  series1->addDatum(20, 43);
//  series1->addDatum(30, 33);
//  series1->addDatum(40, 21);
//  series1->addDatum(50, 33);
//
//  auto series2 = new Series2D<double, double>("myseries1");
//  series2->addDatum(10, 19);
//  series2->addDatum(15, 18);
//  series2->addDatum(20, 22);
//  series2->addDatum(30, 23);
//  series2->addDatum(40, 18);
//  series2->addDatum(50, 21);
//
//  auto series3 = new Series2D<std::string, double>("myseries");
//  series3->addDatum("A", 34);
//  series3->addDatum("B", 38);
//  series3->addDatum("C", 43);
//  series3->addDatum("D", 33);
//  series3->addDatum("E", 21);
//  series3->addDatum("F", 33);
//
//  auto series4 = new Series2D<std::string, double>("myseries");
//  series4->addDatum("A", 19);
//  series4->addDatum("B", 18);
//  series4->addDatum("C", 22);
//  series4->addDatum("D", 23);
//  series4->addDatum("E", 18);
//  series4->addDatum("F", 21);
//
//  ContinuousDomain<double> x_domain(10, 50, false);
//  ContinuousDomain<double> y_domain(0, 50, false);
//
//  PlotLayout canvas;
//
//  auto line_chart = canvas.addChart<LineChart2D<double, double>>(
//      &x_domain, &y_domain);
//
//  line_chart->addSeries(series1);
//  line_chart->addSeries(series2);
//  line_chart->addAxis(AxisDefinition::TOP)->setTitle("x1");
//  line_chart->addAxis(AxisDefinition::LEFT)->setTitle("y1");
//
//  auto point_chart = canvas.addChart<PointChart2D<double, double>>(
//      &x_domain, &y_domain);
//  point_chart->addSeries(series1);
//  point_chart->addSeries(series2);
//
//  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
//      BarChart::O_VERTICAL,
//      false);
//
//  bar_chart->addSeries(series3);
//  bar_chart->addSeries(series4);
//  bar_chart->addAxis(AxisDefinition::RIGHT)->setTitle("x2");
//  bar_chart->addAxis(AxisDefinition::BOTTOM)->setTitle("y2");
//
//  auto bar_domain = dynamic_cast<ContinuousDomain<double>*>(
//      bar_chart->getDomain(AnyDomain::DIM_Y));
//  EXPECT_TRUE(bar_domain != nullptr);
//  bar_domain->setMin(0);
//  bar_domain->setMax(200);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestMultiChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_simple_area_chart_(
//    &ChartTest, "TestSimpleAreaChart", [] () {
//  auto series1 = new Series2D<double, double>("myseries1");
//  series1->addDatum(10, 34);
//  series1->addDatum(15, 38);
//  series1->addDatum(20, 43);
//  series1->addDatum(30, 33);
//  series1->addDatum(40, 21);
//  series1->addDatum(50, 33);
//  series1->setDefaultProperty(Series::P_LINE_STYLE, "solid");
//  series1->setDefaultProperty(Series::P_POINT_STYLE, "circle");
//
//  ContinuousDomain<double> x_domain(10, 50, false);
//  ContinuousDomain<double> y_domain(0, 50, false);
//
//  PlotLayout canvas;
//  auto area_chart = canvas.addChart<AreaChart2D<double, double>>(
//      &x_domain, &y_domain);
//  area_chart->addSeries(series1);
//  area_chart->addAxis(AxisDefinition::TOP);
//  area_chart->addAxis(AxisDefinition::RIGHT);
//  area_chart->addAxis(AxisDefinition::BOTTOM);
//  area_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestSimpleAreaChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_range_area_chart_(
//    &ChartTest, "TestRangeAreaChart", [] () {
//  auto series1 = new Series3D<double, double, double>("myseries1");
//  series1->addDatum(10, -34, 34);
//  series1->addDatum(15, -30, 38);
//  series1->addDatum(20, -20, 43);
//  series1->addDatum(30, -24, 33);
//  series1->addDatum(40, -18, 21);
//  series1->addDatum(50, -32, 33);
//  series1->setDefaultProperty(Series::P_LINE_STYLE, "solid");
//  series1->setDefaultProperty(Series::P_POINT_STYLE, "circle");
//
//  ContinuousDomain<double> x_domain(10, 50, false);
//  ContinuousDomain<double> y_domain(-50, 50, false);
//
//  PlotLayout canvas;
//  auto area_chart = canvas.addChart<AreaChart3D<double, double, double>>(
//      &x_domain, &y_domain);
//  area_chart->addSeries(series1);
//  area_chart->addAxis(AxisDefinition::TOP);
//  area_chart->addAxis(AxisDefinition::RIGHT);
//  area_chart->addAxis(AxisDefinition::BOTTOM);
//  area_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestRangeAreaChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_multi_range_area_chart_(
//    &ChartTest, "TestMultiRangeAreaChart", [] () {
//  auto series1 = new Series3D<double, double, double>("myseries1");
//  series1->addDatum(10, -34, 34);
//  series1->addDatum(15, -30, 38);
//  series1->addDatum(20, -20, 43);
//  series1->addDatum(30, -24, 33);
//  series1->addDatum(40, -18, 21);
//  series1->addDatum(50, -32, 33);
//
//  auto series2 = new Series3D<double, double, double>("myseries2");
//  series2->addDatum(10, -24, 31);
//  series2->addDatum(15, -20, 33);
//  series2->addDatum(20, -15, 38);
//  series2->addDatum(30, -20, 27);
//  series2->addDatum(40, -7, 13);
//  series2->addDatum(50, -18, 30);
//
//  ContinuousDomain<double> x_domain(10, 50, false);
//  ContinuousDomain<double> y_domain(-50, 50, false);
//
//  PlotLayout canvas;
//  auto area_chart = canvas.addChart<AreaChart3D<double, double, double>>(
//      &x_domain, &y_domain);
//  area_chart->addSeries(series1);
//  area_chart->addSeries(series2);
//  area_chart->addAxis(AxisDefinition::TOP);
//  area_chart->addAxis(AxisDefinition::RIGHT);
//  area_chart->addAxis(AxisDefinition::BOTTOM);
//  area_chart->addAxis(AxisDefinition::LEFT);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestMultiRangeAreaChart_out.svg");
//});
//
//static plotfx::test::UnitTest::TestCase __test_multi_range_area_line_(
//    &ChartTest, "TestMultiRangeAreaLineChart", [] () {
//  auto series1 = new Series3D<double, double, double>("myseries1");
//  series1->addDatum(10, -34, 34);
//  series1->addDatum(15, -30, 38);
//  series1->addDatum(20, -20, 43);
//  series1->addDatum(30, -24, 33);
//  series1->addDatum(40, -18, 21);
//  series1->addDatum(50, -32, 33);
//
//  auto series2 = new Series3D<double, double, double>("myseries2");
//  series2->addDatum(10, -24, 31);
//  series2->addDatum(15, -20, 33);
//  series2->addDatum(20, -15, 38);
//  series2->addDatum(30, -20, 27);
//  series2->addDatum(40, -7, 13);
//  series2->addDatum(50, -18, 30);
//
//  auto series3 = new Series2D<double, double>("myseries3");
//  series3->addDatum(10, 4);
//  series3->addDatum(15, 8);
//  series3->addDatum(20, 13);
//  series3->addDatum(30, 2);
//  series3->addDatum(40, 1);
//  series3->addDatum(50, 9);
//  series3->setDefaultProperty(Series::P_COLOR, "black");
//
//  ContinuousDomain<double> x_domain(10, 50, false);
//  ContinuousDomain<double> y_domain(-50, 50, false);
//
//  PlotLayout canvas;
//  auto area_chart = canvas.addChart<AreaChart3D<double, double, double>>(
//      &x_domain, &y_domain);
//  area_chart->addSeries(series1);
//  area_chart->addSeries(series2);
//  area_chart->addAxis(AxisDefinition::TOP);
//  area_chart->addAxis(AxisDefinition::RIGHT);
//  area_chart->addAxis(AxisDefinition::BOTTOM);
//  area_chart->addAxis(AxisDefinition::LEFT);
//
//  auto line_chart = canvas.addChart<LineChart2D<double, double>>(
//      &x_domain, &y_domain);
//  line_chart->addSeries(series3);
//
//  compareChart(
//      &canvas,
//      "ChartTest_TestMultiRangeAreaLineChart_out.svg");
//});
