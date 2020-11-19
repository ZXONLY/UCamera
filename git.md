# GIT教程

## 0. 基础知识

### working Directory(工作区)

### Repository(版本库)

git add是将文件加入到working directory, 而git commit 是把工作区的所有修改提交到版本库中

## 1. 创建版本库

#### git init

通过```git init ```指令可以把目标目录变成git管理的仓库。通过该指令你可以看到在目录下生成了.git的文件夹

#### git add

```git add```命令是将目的文件加到git仓库中

```git
git add readme.txt //也可以是绝对路径
```

#### git commit

```git commit```是将文件提交到仓库中

```git
git commit -m "write a readme file"
```

#### git push

```git push```将本地代码库的所有内容推送到远程库中

## 2.版本管理

### 2.1 版本回退

#### git log

```git log```显示的是从远到近的提交日志。

```git log --pretty=oneline```可以把commit日志显示到一行

#### git reset

```git reset --head HEAD^```可以返回到上一个版本

```git reset --head 版本号```可以回退到对应的版本号

#### git reflog

该指令可以记录之前的命令

#### git status

通过该指令可以清晰的看到哪些文件被修改，哪些文件是被添加的，哪些文件的修改有没有commit

### 2.2 管理修改

#### git diff

```git diff HEAD -- 文件名```可以查看工作区和版本库里面最新版本的区别

#### 撤销修改

```git checkout  --文件名```把对应文件名的文件在工作区的修改全部撤销，相当于回到最近一次git commit或git add的状态

```git checkout 分支名```可以切换到对应的分支中

```git reset HEAD 文件名  ```可以把对应文件名的文件的修改撤销掉

#### git rm

删除对应的文件