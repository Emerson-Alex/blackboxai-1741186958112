#include "SETimelineTestReportTemplate.h"

FSETimelineTestReportTemplate& FSETimelineTestReportTemplate::Get()
{
    static FSETimelineTestReportTemplate Instance;
    return Instance;
}

FSETimelineTestReportTemplate::FSETimelineTestReportTemplate()
{
}

const TCHAR* FSETimelineTestReportTemplate::HTMLTemplate = TEXT(R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{REPORT_TITLE}}</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        {{CSS_STYLES}}
    </style>
</head>
<body>
    <div class="container">
        {{HEADER}}
        {{SUMMARY}}
        {{TEST_RESULTS}}
        {{PERFORMANCE_DATA}}
        {{SCREENSHOTS}}
        {{SYSTEM_INFO}}
        {{FOOTER}}
    </div>
    <script>
        {{JAVASCRIPT}}
    </script>
</body>
</html>
)");

const TCHAR* FSETimelineTestReportTemplate::CSSStyles = TEXT(R"(
:root {
    --primary-color: #007bff;
    --secondary-color: #6c757d;
    --success-color: #28a745;
    --warning-color: #ffc107;
    --error-color: #dc3545;
    --info-color: #17a2b8;
    --light-color: #f8f9fa;
    --dark-color: #343a40;
}

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: Arial, sans-serif;
    line-height: 1.6;
    color: #333;
    background-color: #f4f4f4;
}

.container {
    max-width: 1200px;
    margin: 0 auto;
    padding: 20px;
}

.header {
    background-color: var(--primary-color);
    color: white;
    padding: 20px;
    border-radius: 5px;
    margin-bottom: 20px;
}

.summary {
    background-color: white;
    padding: 20px;
    border-radius: 5px;
    margin-bottom: 20px;
    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.test-results {
    background-color: white;
    padding: 20px;
    border-radius: 5px;
    margin-bottom: 20px;
    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.performance {
    background-color: white;
    padding: 20px;
    border-radius: 5px;
    margin-bottom: 20px;
    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.screenshots {
    background-color: white;
    padding: 20px;
    border-radius: 5px;
    margin-bottom: 20px;
    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.system-info {
    background-color: white;
    padding: 20px;
    border-radius: 5px;
    margin-bottom: 20px;
    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.footer {
    text-align: center;
    padding: 20px;
    color: var(--secondary-color);
}

.status {
    padding: 5px 10px;
    border-radius: 3px;
    font-weight: bold;
}

.status-passed {
    background-color: var(--success-color);
    color: white;
}

.status-failed {
    background-color: var(--error-color);
    color: white;
}

.status-skipped {
    background-color: var(--warning-color);
    color: white;
}

.chart-container {
    position: relative;
    height: 300px;
    margin-bottom: 20px;
}

.screenshot {
    max-width: 300px;
    margin: 10px;
    cursor: pointer;
    transition: transform 0.2s;
}

.screenshot:hover {
    transform: scale(1.05);
}

.modal {
    display: none;
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: rgba(0,0,0,0.8);
    z-index: 1000;
}

.modal-content {
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    max-width: 90%;
    max-height: 90%;
}

.close {
    position: absolute;
    top: 15px;
    right: 15px;
    color: white;
    font-size: 30px;
    cursor: pointer;
}

table {
    width: 100%;
    border-collapse: collapse;
    margin-bottom: 20px;
}

th, td {
    padding: 12px;
    text-align: left;
    border-bottom: 1px solid #ddd;
}

th {
    background-color: var(--light-color);
    font-weight: bold;
}

tr:hover {
    background-color: #f5f5f5;
}

.details {
    display: none;
    padding: 20px;
    background-color: #f8f9fa;
    border-radius: 5px;
    margin: 10px 0;
}

.show-details {
    cursor: pointer;
    color: var(--primary-color);
    text-decoration: underline;
}
)");

const TCHAR* FSETimelineTestReportTemplate::JavaScript = TEXT(R"(
function showScreenshot(src) {
    const modal = document.getElementById('screenshot-modal');
    const modalImg = document.getElementById('modal-image');
    modal.style.display = 'block';
    modalImg.src = src;
}

function closeModal() {
    const modal = document.getElementById('screenshot-modal');
    modal.style.display = 'none';
}

function toggleDetails(id) {
    const details = document.getElementById(id);
    if (details.style.display === 'none') {
        details.style.display = 'block';
    } else {
        details.style.display = 'none';
    }
}

function createChart(ctx, data, options) {
    return new Chart(ctx, {
        type: options.type || 'bar',
        data: {
            labels: data.labels,
            datasets: [{
                label: options.label || '',
                data: data.values,
                backgroundColor: options.backgroundColor || 'rgba(0, 123, 255, 0.5)',
                borderColor: options.borderColor || 'rgba(0, 123, 255, 1)',
                borderWidth: 1
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                y: {
                    beginAtZero: true
                }
            }
        }
    });
}

document.addEventListener('DOMContentLoaded', function() {
    // Initialize charts
    const testResultsChart = createChart(
        document.getElementById('test-results-chart'),
        {{TEST_RESULTS_DATA}},
        {
            type: 'pie',
            label: 'Test Results',
            backgroundColor: ['#28a745', '#dc3545', '#ffc107']
        }
    );

    const performanceChart = createChart(
        document.getElementById('performance-chart'),
        {{PERFORMANCE_DATA}},
        {
            type: 'line',
            label: 'Performance Metrics'
        }
    );

    const durationChart = createChart(
        document.getElementById('duration-chart'),
        {{DURATION_DATA}},
        {
            type: 'bar',
            label: 'Test Duration'
        }
    );

    const memoryChart = createChart(
        document.getElementById('memory-chart'),
        {{MEMORY_DATA}},
        {
            type: 'line',
            label: 'Memory Usage'
        }
    );
});
)");

FString FSETimelineTestReportTemplate::GetHTMLTemplate() const
{
    return LoadTemplate(HTMLTemplate);
}

FString FSETimelineTestReportTemplate::GetCSSStyles() const
{
    return LoadTemplate(CSSStyles);
}

FString FSETimelineTestReportTemplate::GetJavaScript() const
{
    return LoadTemplate(JavaScript);
}

FString FSETimelineTestReportTemplate::GetHeaderTemplate() const
{
    static const TCHAR* Template = TEXT(R"(
<div class="header">
    <h1>{{REPORT_TITLE}}</h1>
    <p>{{REPORT_DESCRIPTION}}</p>
    <p class="timestamp">Generated on {{TIMESTAMP}}</p>
</div>
)");
    return LoadTemplate(Template);
}

FString FSETimelineTestReportTemplate::GetSummaryTemplate() const
{
    static const TCHAR* Template = TEXT(R"(
<div class="summary">
    <h2>Summary</h2>
    <div class="summary-stats">
        <p>Total Tests: {{TOTAL_TESTS}}</p>
        <p>Passed: <span class="status status-passed">{{PASSED_TESTS}}</span></p>
        <p>Failed: <span class="status status-failed">{{FAILED_TESTS}}</span></p>
        <p>Skipped: <span class="status status-skipped">{{SKIPPED_TESTS}}</span></p>
        <p>Total Duration: {{TOTAL_DURATION}}</p>
    </div>
    <div class="chart-container">
        <canvas id="test-results-chart"></canvas>
    </div>
</div>
)");
    return LoadTemplate(Template);
}

FString FSETimelineTestReportTemplate::LoadTemplate(const TCHAR* Template) const
{
    return FString(Template);
}

FString FSETimelineTestReportTemplate::ReplaceVariable(const FString& Template, const TCHAR* Variable, const FString& Value) const
{
    return Template.Replace(Variable, *Value);
}

FString FSETimelineTestReportTemplate::FormatChartData(const TArray<FString>& Labels, const TArray<float>& Values) const
{
    FString Result = TEXT("{\"labels\":[");
    for (int32 i = 0; i < Labels.Num(); ++i)
    {
        if (i > 0) Result += TEXT(",");
        Result += FString::Printf(TEXT("\"%s\""), *Labels[i]);
    }
    Result += TEXT("],\"values\":[");
    for (int32 i = 0; i < Values.Num(); ++i)
    {
        if (i > 0) Result += TEXT(",");
        Result += FString::Printf(TEXT("%.2f"), Values[i]);
    }
    Result += TEXT("]}");
    return Result;
}

// Implement other methods similarly...
