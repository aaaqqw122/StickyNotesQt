main.cpp：程序入口。

存数据
note_data.*：便签的数据结构定义与实现。
note_json.*：便签的 JSON 序列化与反序列化相关逻辑。



主界面下部分
note_list_widget.*：便签列表的 UI 组件，负责展示所有便签。note_list_widget 负责整体布局和列表背景；

主界面
note_manager.*：便签的管理逻辑，可能负责增删改查等操作。NoteWidget 负责每个便签的内容和单独背景。

便签
note_widget.*：单个便签的 UI 组件。

设置页
settings_widget.*：设置界面相关逻辑。