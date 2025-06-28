# 垂直Tab按钮解决方案

## 问题解决

### 原始问题

使用 `QTabBar::RoundedWest` 会导致文字也旋转90度，这不是我们想要的效果。

### 解决方案

使用自定义的按钮组（QButtonGroup）来模拟Tab效果，实现：

- ✅ 垂直排列的视觉效果
- ✅ 文字保持水平方向
- ✅ Tab样式的外观和交互

## 实现细节

### 1. 组件替换

- **原来**: QTabBar + setShape(RoundedWest)
- **现在**: QPushButton + QButtonGroup

### 2. 核心代码

```cpp
// 创建可切换的按钮
QPushButton *showButton = new QPushButton(tr("显示"));
QPushButton *hideButton = new QPushButton(tr("不显示"));

// 设置为可切换状态
showButton->setCheckable(true);
hideButton->setCheckable(true);
showButton->setChecked(true); // 默认选择

// 创建按钮组确保互斥选择
QButtonGroup *buttonGroup = new QButtonGroup(this);
buttonGroup->addButton(showButton, 0);  // ID: 0
buttonGroup->addButton(hideButton, 1);  // ID: 1
buttonGroup->setExclusive(true);
```

### 3. 样式设计

```css
QPushButton[objectName="tabButton"] {
    background-color: #e0e0e0; /* 默认灰色背景 */
    border: 1px solid #bdbdbd; /* 灰色边框 */
    border-radius: 4px; /* 圆角 */
    padding: 8px 12px; /* 内边距 */
    margin: 2px 0px; /* 垂直间距 */
}

QPushButton[objectName="tabButton"]:checked {
    background-color: #2196f3; /* 选中时蓝色背景 */
    color: white; /* 白色文字 */
}
```

### 4. 功能逻辑

```cpp
// 判断是否显示代码
bool shouldShowCode = (codeVisibilityButtonGroup != nullptr &&
                      codeVisibilityButtonGroup->checkedId() == 0);
```

## 视觉效果

### 布局样式

```
目标代码
┌─────────┐
│  显示   │  ← 选中状态（蓝色背景）
├─────────┤
│ 不显示  │  ← 未选中状态（灰色背景）
└─────────┘
```

### 交互状态

- **默认状态**: 灰色背景，深色文字
- **选中状态**: 蓝色背景，白色文字
- **悬停状态**: 浅灰色背景
- **选中+悬停**: 深蓝色背景

## 优势对比

| 特性       | QTabBar::RoundedWest | 自定义按钮组  |
| ---------- | -------------------- | ------------- |
| 文字方向   | 旋转90度 ❌          | 保持水平 ✅   |
| 垂直排列   | 是 ✅                | 是 ✅         |
| 样式自定义 | 有限                 | 完全自定义 ✅ |
| 交互反馈   | 基本                 | 丰富 ✅       |
| 代码复杂度 | 简单                 | 中等          |

## 兼容性

### 向后兼容

- 保留了 `codeVisibilityTabBar` 成员变量（设为nullptr）
- 新增 `codeVisibilityButtonGroup` 成员变量
- 修改了判断逻辑但保持功能一致

### 扩展性

- 可以轻松添加更多选项
- 可以添加图标
- 可以实现更复杂的交互效果

## 使用说明

1. **编译项目**：新的按钮组会自动应用样式
2. **选择选项**：点击不同按钮可以切换状态
3. **视觉反馈**：按钮会有明显的选中和悬停效果
4. **功能保持**：与原来的Tab Bar功能完全一致

这个解决方案完美地平衡了视觉效果和用户体验，实现了真正的垂直排列而不影响文字的可读性。
