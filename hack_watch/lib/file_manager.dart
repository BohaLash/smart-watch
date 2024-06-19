import 'package:flutter/material.dart';
import 'dart:async';

class FileManager extends StatefulWidget {
	List< List<String> > tree;
	Function(String) callback;

	FileManager(this.tree, this.callback);

  @override
  _FileManagerState createState() => _FileManagerState();
}

class _FileManagerState extends State<FileManager> {
  List<String> titles = ["Math", "Phythics", "Chemistry", "Biology", "Geogaphy", "History", "Ukrainian", "English", "Others"];

  TextEditingController _titleController;
  TextEditingController _textController;

  void initState() {
    super.initState();
    _titleController = new TextEditingController();
    _textController = new TextEditingController();
  }

  void dispose() {
    _titleController.dispose();
    _textController.dispose();
    super.dispose();
  }

	@override
  Widget build(BuildContext context) {
    return ListView.builder(
      padding: const EdgeInsets.all(10),
      itemCount: widget.tree.length,
      itemBuilder: (BuildContext context, int index) {
        return ListTile(
          title: Row(
            children: <Widget>[
              Icon(Icons.folder_open),
              Text(
                '   ' + titles[index],
                style: TextStyle(
                  fontWeight: FontWeight.bold,
                ),
              ),
            ],
          ),
          subtitle: Column(
            children: List<Widget>.generate(widget.tree[index].length, (int i) => _getColumnTile(index, i)),
          ),
          trailing: IconButton(
            icon: Icon(Icons.add),
            onPressed: () => _createSendDialog(index),
          ),
        );
      }
    );
  }

  Widget _getColumnTile(int i, int j) {
    return ListTile(
      leading: Icon(Icons.format_align_left),
      title: Text(widget.tree[i][j]),
      trailing: IconButton(
        icon: Icon(Icons.delete),
        onPressed: () => _createDeleteDialog(i, j, widget.tree[i][j]),
      ),
    );
  }

  _sendNewFile(int index, String text) {
    print(text.length);
    widget.callback(text.length.toString() + index.toString() + '!' + text);
    print(text);
  }

  Future<void> _createDeleteDialog(int i, int j, String title) async {
    return await showDialog<void>(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text('Delete ' + title + '?'),
          content: const Text('You cannot undo this action!'),
          actions: <Widget>[
            FlatButton(
              onPressed: () => Navigator.pop(context),
              child: const Text('Cancel'),
            ),
            FlatButton(
              onPressed: () {
                widget.callback(j.toString() + i.toString() + '#');
                Navigator.pop(context);
              },
              child: const Text('Delete'),
            ),
          ],
        );
      },
    );
  }

  Future<void> _createSendDialog(int index) async {
    return showDialog<void>(
      context: context,
      barrierDismissible: true,
      builder: (BuildContext context) {
        return Dialog(
          child: Container(
            height: 330.0,
            child: Padding(
              padding: EdgeInsets.all(30.0),
              child: Column(
                children: <Widget> [
                  Text(
                    'Add File to ' + titles[index] + ' Folder',
                    style: TextStyle(
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                  Expanded(
                    child: Center (
                      child: Column(
                        children: <Widget> [
                          TextField(
                            controller: _titleController,
                            decoration: InputDecoration(
                              labelText: 'Title',
                            ),
                            // keyboardType: TextInputType.multiline,
                            // minLines: 1,
                            // maxLines: 5,
                            autofocus: true,
                          ),
                          TextField(
                            controller: _textController,
                            decoration: InputDecoration(
                              labelText: 'Text',
                            ),
                            keyboardType: TextInputType.multiline,
                            minLines: 1,
                            maxLines: 5,
                            autofocus: false,
                          ),
                        ],
                      ),
                    ),
                  ),
                  RaisedButton(
                    onPressed: () => _sendNewFile(index, _titleController.text + '@' + _textController.text),
                    child: Text(
                      'Send',
                    ),
                  ),
                ],
              ),
            ),
          ),
        );
      },
    );
  }

  // Future<void> _askedToLead() async {
  //   switch (await showDialog<Department>(
  //     context: context,
  //     builder: (BuildContext context) {
  //       return SimpleDialog(
  //         title: const Text('Select assignment'),
  //         children: <Widget>[
  //           SimpleDialogOption(
  //             onPressed: () { Navigator.pop(context, Department.treasury); },
  //             child: const Text('Treasury department'),
  //           ),
  //           SimpleDialogOption(
  //             onPressed: () { Navigator.pop(context, Department.state); },
  //             child: const Text('State department'),
  //           ),
  //         ],
  //       );
  //     }
  //   )) {
  //     case Department.treasury:
  //       // Let's go.
  //       // ...
  //     break;
  //     case Department.state:
  //       // ...
  //     break;
  //   }
  // }

}
