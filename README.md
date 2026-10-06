# C语言练习代码仓库

这是你学习C语言的代码备份仓库，用Git管理版本，用GitHub做云端备份。

---

## 📁 这个仓库里有什么

| 文件 | 内容 |
|------|------|
| `fib_loop.c` | 循环版本斐波那契数列 |
| `two_d_array.c` | 二维数组3×3矩阵练习 |
| `pointer_basic.c` | 指针基础（swap交换、指针遍历数组） |
| `.gitignore` | Git忽略规则（不上传编译产物） |

---

## 🚀 日常使用：改完代码怎么上传GitHub

每次写完/改完代码，打开 **Git Bash**（在这个文件夹里右键 → "Git Bash Here"），然后执行下面三步：

### 第一步：添加改动到暂存区
```bash
git add .
```
> `.` 代表当前文件夹所有改动的文件

### 第二步：记录这次改动（写清楚改了什么）
```bash
git commit -m "说明这次改了什么，比如：修复斐波那契return位置bug"
```
> ⚠️ `-m` 后面的引号里一定要写清楚改了什么，以后回头看才知道

### 第三步：推送到GitHub云端
```bash
git push
```
> 第一次推送可能会弹浏览器登录GitHub，点授权就行

---

## 📋 常用命令速查

| 命令 | 作用 | 什么时候用 |
|------|------|-----------|
| `git status` | 查看当前改了哪些文件 | 不确定有没有保存时 |
| `git add .` | 把所有改动加入暂存区 | 每次commit前 |
| `git commit -m "说明"` | 保存一个版本 | 写完一个小功能就commit一次 |
| `git push` | 上传到GitHub | commit完之后 |
| `git pull` | 下载GitHub上最新的代码 | 换电脑、或者网页上改过代码后 |
| `git log` | 查看历史提交记录 | 想看看以前改了什么 |
| `git log --oneline` | 简洁版历史记录 | 只看提交信息 |

---

## ⚠️ 新手必看提醒

### 1. 文件夹路径不要有中文和空格
这个仓库的路径最好是英文，比如 `D:\code\c-language-practice`
> 中文路径在Git里容易出各种奇怪问题

### 2. 不要上传 .exe 编译产物
`.gitignore` 已经帮你配置好了，`.exe` 文件不会被上传。
> 你不需要手动管，Git自动帮你忽略

### 3. commit 备注要写清楚
❌ 不要写：`update`、`111`、`修改`
✅ 要写：`增加二维数组矩阵打印功能`、`修复斐波那契递归死循环bug`

### 4. 多commit，少攒大改动
写完一个小功能就commit一次，不要攒着几十行代码改完才提交。
> 这样出问题了，回退版本才方便

### 5. 改代码前先 pull
如果你在别的电脑上也写代码，开始写之前先 `git pull` 拉一下最新代码，避免冲突。

---

## 🔧 第一次连接GitHub（需要你自己操作）

### 步骤1：注册GitHub账号
打开 https://github.com/ ，用邮箱注册一个免费账号。

### 步骤2：在GitHub上新建仓库
1. 登录后点右上角 `+` → `New repository`
2. 仓库名填：`c-language-practice`
3. 选 `Public`（公开）
4. **不要勾选** "Add a README"（因为本地已经有了）
5. 点 `Create repository`

### 步骤3：把本地仓库关联到GitHub
在Git Bash里执行（把下面的用户名换成你自己的）：
```bash
git remote add origin https://github.com/你的用户名/c-language-practice.git
git branch -M main
git push -u origin main
```

> 第一次push会弹浏览器让你登录GitHub，授权一次就行，以后不用重复登录。

---

## 📝 怎么写新的C语言练习

1. 在这个文件夹里新建 `.c` 文件，比如 `sort_bubble.c`
2. 写完代码，在VS里编译测试通过
3. 打开Git Bash，执行三板斧：
   ```bash
   git add .
   git commit -m "新增冒泡排序算法练习"
   git push
   ```
4. 刷新GitHub网页，就能看到你的新代码了

---

## 🆘 常见问题

**Q: git push 报错说拒绝了？**
A: 先执行 `git pull` 拉一下云端代码，解决冲突后再push。

**Q: 改错代码了想回到上一个版本怎么办？**
A: 执行 `git log --oneline` 找到上一个版本的编号，然后：
```bash
git checkout 那个版本号 -- 文件名.c
```

**Q: 我不小心把不想上传的文件add了怎么办？**
A: 执行 `git reset HEAD 文件名` 取消暂存，不会删你的文件。

---

## 🎯 学习进度记录

- [x] Git安装 + 本地仓库初始化
- [x] 斐波那契数列（循环版）
- [x] 二维数组基础
- [x] 指针基础
- [ ] 冒泡排序
- [ ] 字符串处理
- [ ] 结构体
- [ ] 链表入门
