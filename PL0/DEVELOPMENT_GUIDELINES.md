# PL0 编译器项目开发规范

## 概述

本文档定义了 PL0 编译器项目的代码风格和开发规范。

## 代码格式化工具

### Prettier (用于 HTML, JSON, QSS 文件)

- 配置文件: `.prettierrc`
- 忽略文件: `.prettierignore`
- 支持的文件类型: HTML, JSON, CSS/QSS

### Clang-Format (用于 C++ 文件)

- 配置文件: `.clang-format`
- 支持的文件类型: .cpp, .h, .hpp

### EditorConfig (通用编辑器配置)

- 配置文件: `.editorconfig`
- 适用于所有文件类型的基本格式化规则

## C++ 代码规范

### 命名约定

- **类名**: 使用 PascalCase (如 `MainWindow`, `PL0Compiler`)
- **函数名**: 使用 camelCase (如 `parseExpression`, `generateCode`)
- **变量名**: 使用 camelCase (如 `tokenIndex`, `currentSymbol`)
- **常量**: 使用 UPPER_SNAKE_CASE (如 `MAX_SYMBOL_LENGTH`)
- **私有成员变量**: 可选择添加 `m_` 前缀 (如 `m_tokenList`)

### 代码风格

- **缩进**: 4个空格，不使用 Tab
- **行长度**: 最大120字符
- **大括号**: Allman 风格（大括号单独一行）

```cpp
if (condition)
{
    // 代码
}
```

### 文件组织

```cpp
// 1. 版权声明（如果需要）
// 2. #include 指令（系统头文件在前，项目头文件在后）
// 3. 命名空间声明
// 4. 类声明
// 5. 实现代码
```

### 注释规范

- 使用 `//` 进行单行注释
- 使用 `/* */` 进行多行注释
- 重要函数添加文档注释：

```cpp
/**
 * @brief 解析表达式
 * @param tokenList 词法单元列表
 * @return 解析结果
 */
ParseResult parseExpression(const TokenList& tokenList);
```

## HTML/Web 文件规范

### HTML

- **缩进**: 2个空格
- **行长度**: 最大100字符
- **属性**: 使用双引号
- **标签**: 小写

### QSS (Qt样式表)

- **缩进**: 4个空格
- **选择器**: 每个选择器独占一行
- **属性**: 按字母顺序排列（可选）

## JSON 文件规范

- **缩进**: 2个空格
- **字符串**: 使用双引号
- **尾随逗号**: 在对象和数组中使用（ES5风格）

## 项目文件组织

### 源代码文件

- `*.cpp`: C++ 实现文件
- `*.h`: C++ 头文件
- `*.qss`: Qt 样式表文件
- `*.html`: Web 界面文件

### 测试文件

- `test/`: 测试用例目录
- `*.PL0`: PL0 源代码文件
- `*.COD`: 编译输出文件

### 构建文件

- `build/`: 构建输出目录
- `debug/`, `release/`: 调试和发布版本
- `Makefile*`: 构建脚本

## Git 提交规范

### 提交信息格式

```
<类型>(<范围>): <描述>

[可选的正文]

[可选的脚注]
```

### 类型

- `feat`: 新功能
- `fix`: 修复bug
- `docs`: 文档变更
- `style`: 代码风格调整（不影响功能）
- `refactor`: 重构代码
- `test`: 测试相关
- `chore`: 构建过程或辅助工具的变动

### 示例

```
feat(parser): 添加 for 循环语法支持

实现了 PL0 语言的 for 循环语法解析和代码生成功能。

Closes #123
```

## 开发工作流

1. **开发前**: 确保安装了支持的格式化工具
2. **编码时**: 遵循代码规范，使用一致的命名和风格
3. **提交前**: 运行格式化工具检查代码风格
4. **测试**: 在 `test/` 目录下添加相应的测试用例
5. **文档**: 更新相关的文档和注释

## 工具推荐

### VS Code 扩展

- C/C++ (Microsoft)
- Prettier - Code formatter
- EditorConfig for VS Code
- Qt tools

### 命令行工具

```bash
# 安装 Prettier (需要 Node.js)
npm install --global prettier

# 格式化所有支持的文件
prettier --write .

# 检查 C++ 代码格式
clang-format --style=file -i *.cpp *.h
```

## 注意事项

1. **生成的文件**: `moc_*.cpp`, `qrc_*.cpp` 等 Qt 生成的文件不需要手动格式化
2. **构建文件**: `build/`, `debug/`, `release/` 目录下的文件由构建系统管理
3. **测试文件**: `.COD` 文件是编译器输出，不需要格式化
4. **兼容性**: 确保代码风格在不同操作系统和编辑器中保持一致

---

遵循这些规范将有助于保持代码库的一致性和可维护性。
