# Tab Bar组件替换说明

## 修改内容

### 1. 界面组件更换
- **原来**: 使用QRadioButton（单选按钮）+ QGroupBox
- **现在**: 使用QTabBar（标签栏）+ QGroupBox

### 2. 用户体验改进
- **更直观**: Tab Bar比Radio Button更适合二元选择
- **更现代**: 符合现代UI设计趋势
- **更美观**: 标签式设计更加简洁明了

### 3. 代码修改点

#### mainwindow.h
- 添加 `QTabBar` 前向声明
- 将 `QRadioButton *visibleRadioButton, *invisibleRadioButton` 替换为 `QTabBar *codeVisibilityTabBar`
- 重命名 `isVisibleGroupBox` 为 `codeOptionsGroupBox`

#### mainwindow.cpp
- 添加 `#include <QTabBar>`
- 修改 `createPL0Page()` 函数中的UI创建逻辑
- 使用 `addTab()` 添加"显示"和"不显示"选项
- 设置默认选中第一个标签（显示）

#### PL0.cpp
- 添加 `#include <QTabBar>`
- 修改 `ListCode()` 函数中的判断逻辑
- 从 `visibleRadioButton->isChecked()` 改为 `codeVisibilityTabBar->currentIndex() == 0`

#### modern_style.qss
- 添加专门的Tab Bar样式
- 实现Material Design风格的标签效果
- 支持选中、悬停状态的视觉反馈

### 4. 样式特性
- **默认状态**: 灰色背景，深灰色文字
- **选中状态**: 蓝色背景，白色文字
- **悬停状态**: 浅灰色背景，边框颜色变化
- **圆角设计**: 符合现代UI规范

### 5. 功能逻辑
- **索引0**: "显示" - 显示目标代码
- **索引1**: "不显示" - 隐藏目标代码
- **默认选择**: "显示"状态
- **工具提示**: "选择是否显示生成的目标代码"

## 使用说明
1. 编译项目时Tab Bar会自动应用新样式
2. 点击不同标签可以切换目标代码的显示状态
3. 标签会有视觉反馈（颜色变化、悬停效果）
4. 功能与原来的Radio Button完全一致

## 优势对比
| 特性 | Radio Button | Tab Bar |
|------|-------------|---------|
| 视觉层次 | 较弱 | 更强 |
| 现代感 | 一般 | 很好 |
| 空间利用 | 垂直排列 | 水平排列 |
| 交互反馈 | 基本 | 丰富 |
| 样式自定义 | 有限 | 灵活 |
