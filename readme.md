改变了一下项目结构
./res/img 放置图片
./src/各个章节的名字/main.cpp是主程序
./src/各个章节的名字/shader/是shader

偷懒了，不想每次进入新章节都创建新文件夹，还不如直接就开一个新分支

运行方法
make run dir=src下面的某个目录
调试方法
修改./vscode/tasks.json里面的build命令的目录，然后就可以F5使用vscode的调试了

错误
- ```shell
    terminate called after throwing an instance of 'std::ios_base::failure'
    what():  basic_ios::clear
    Makefile:94: recipe for target 'run' failed
    make: *** [run] Error 3
    ```
  - 可能是路径问题
    - 我在vscode的launch.json可以设置cwd，用vscode的code-runner是可以的
    - 但是在用make命令的时候这个设置并不会有效

这个Makefile对于只修改了include里面的文件的情况下是不会认为发生改变的，要改一下main.cpp才会改变