import 'dart:async';
import 'dart:convert';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:hardware_buttons/hardware_buttons.dart';
import 'package:screen_state/screen_state.dart';

import './trackpad.dart';
import './swipepad.dart';
import './file_manager.dart';

class ChatPage extends StatefulWidget {
  final BluetoothDevice server;

  const ChatPage({this.server});

  @override
  _ChatPage createState() => new _ChatPage();
}

class _ChatPage extends State<ChatPage> {
  int i, j, buf;
  int counter = 0;

  final GlobalKey<ScaffoldState> _scaffoldKey = new GlobalKey<ScaffoldState>();

  static final clientID = 0;
  BluetoothConnection connection;

  String _messageBuffer = '';

  bool isConnecting = true;
  bool get isConnected => connection != null && connection.isConnected;

  bool isDisconnecting = false;

  var lastScroll = DateTime.now();
  var lastMessage = DateTime.now();
  var now;

  bool _showSettings = false;
  double _currentDelaySliderValue = 0;
  double _currentSensitySliderValue = 5;
  double _sensity = 5;

  int _currentTab = 0;

  bool asced = false;

  final List<Tab> tabs = <Tab>[
    Tab(text: 'File Manager'),
    Tab(text: 'Swipepad'),
    Tab(text: 'Trackpad'),
  ];

  List<String> l;
  List< List<String> > parsedData;
  List< List<String> > tree = new List< List<String> >();

  callback(text) {
    _sendMessage(text);
  }

  StreamSubscription _volumeButtonSubscription;

  Screen _screen;
  StreamSubscription<ScreenStateEvent> _subscription;
  // StreamSubscription _lockButtonSubscription;

  @override
  void initState() {
    super.initState();

    initPlatformState();

    _volumeButtonSubscription = volumeButtonEvents.listen((VolumeButtonEvent event) {
      print(event.toString());
      if (event == VolumeButtonEvent.VOLUME_UP) {
        _onScroll(true);
      } else if (event == VolumeButtonEvent.VOLUME_DOWN) {
        _onScroll(false);
      } else {
        print('volume buttons error');
      }
    });

    // _lockButtonSubscription = lockButtonEvents.listen((LockButtonEvent event) {
    //   print('lock');
    //   _sendMessage('q');
    // });
    

    BluetoothConnection.toAddress(widget.server.address).then((_connection) {
      print('Connected to the device');
      connection = _connection;
      setState(() {
        isConnecting = false;
        isDisconnecting = false;
      });

      _sendTime();
      _sendMessage('?');

      connection.input.listen(_onDataReceived).onDone(() {
        // Example: Detect which side closed the connection
        // There should be `isDisconnecting` flag to show are we are (locally)
        // in middle of disconnecting process, should be set before calling
        // `dispose`, `finish` or `close`, which all causes to disconnect.
        // If we except the disconnection, `onDone` should be fired as result.
        // If we didn't except this (no flag set), it means closing by remote.
        if (isDisconnecting) {
          print('Disconnecting locally!');
        } else {
          print('Disconnected remotely!');
        }
        if (this.mounted) {
          setState(() {});
        }
      });
    }).catchError((error) {
      print('Cannot connect, exception occured');
      print(error);
    });
  }

  @override
  void dispose() {
    // Avoid memory leak (`setState` after dispose) and disconnect
    if (isConnected) {
      isDisconnecting = true;
      connection.dispose();
      connection = null;
    }

    super.dispose();
    _volumeButtonSubscription?.cancel();
    // _lockButtonSubscription?.cancel();
  }

  @override
  Widget build(BuildContext context) {
    return DefaultTabController(
      length: tabs.length,
      child: Builder(builder: (BuildContext context) {
        final TabController tabController = DefaultTabController.of(context);
        tabController.addListener(() {
          if (!tabController.indexIsChanging) {
            setState(() {
              _currentTab = tabController.index;
              if (_currentTab == 0) {
                if (!asced) {
                  _sendMessage('?');
                  asced = true;
                }
              } else {
                asced = false;
              }
            });
          }
        });
        return Scaffold(
          key: _scaffoldKey,
          appBar: AppBar(
            title: (isConnecting
              ? Text('Connecting to ' + widget.server.name + '...')
              : isConnected
                  ? Text('Live chat with ' + widget.server.name)
                  : Text('Chat log with ' + widget.server.name)
            ),
            actions: <Widget>[
              IconButton(
                icon: const Icon(Icons.settings),
                tooltip: 'Show Settings',
                onPressed: () {
                  setState(() {
                    _showSettings = !_showSettings;
                    // if (value == 0)
                            // _sendMessage('?');
                  });
                  print(_showSettings);
                },
              ),
            ],
            bottom: _showSettings
              ? PreferredSize(
                preferredSize: const Size.fromHeight(250.0),
                child: Column(
                  children: <Widget>[
                    Text(
                      'Delay',
                      style: TextStyle(
                        color: Colors.white,
                        // fontWeight: FontWeight.bold,
                      ),
                    ),
                    Slider(
                      value: _currentDelaySliderValue,
                      min: 0,
                      max: 50,
                      divisions: 10,
                      label: _currentDelaySliderValue.round().toString(),
                      activeColor: Colors.white,
                      inactiveColor: Colors.white,
                      onChanged: (double value) {
                        setState(() {
                          _currentDelaySliderValue = value;
                          
                        });
                      },
                      onChangeEnd: (double value) {
                        _sendMessage(value.round().toString() + 's');
                      }, 
                    ),
                    // SizedBox(
                    //   height: 10.0,
                    // ),
                    Text(
                      'Sensity',
                      style: TextStyle(
                        color: Colors.white,
                        // fontWeight: FontWeight.bold,
                      ),
                    ),
                    Slider(
                      value: _currentSensitySliderValue,
                      min: 1,
                      max: 9,
                      divisions: 10,
                      label: _currentSensitySliderValue.round().toString(),
                      activeColor: Colors.white,
                      inactiveColor: Colors.white,
                      onChanged: (double value) {
                        setState(() {
                          _currentSensitySliderValue = value; 
                        });
                      },
                      onChangeEnd: (double value) {
                        setState(() {
                          _sensity = value; 
                        });
                      }, 
                    ),
                    Padding(
                      padding: EdgeInsets.all(10.0),
                      child: RaisedButton(
                        onPressed: () => _sendTime(),
                        child: const Text('Send Current Time'),
                      ),
                    ),
                    TabBar(
                      tabs: tabs,
                    ),
                  ],
                ),
              )
              : TabBar(
                tabs: tabs,
              ),
          ),
          body: TabBarView(
            physics: NeverScrollableScrollPhysics(),
            children: tabs.map((Tab tab) {
              return SafeArea(
                child: 
                // Text(_currentTab.toString()),
                _getPageWidget(_currentTab),
                // Trackpad(_currentDelaySliderValue.toInt(), _sensity.toInt(), callback),
              );
            }).toList(),
          ),
          // floatingActionButton: FloatingActionButton(
          //   onPressed: () => ,
          //   tooltip: 'Increment Counter',
          //   child: const Icon(Icons.add),
          // ),
        );
      }),
    );
  }

  Widget _getPageWidget(int index) {
    // 
    // _sendMessage('?');

    switch (index) {
      case 0: {
        // print('?');
        // _sendMessage('?');
        return FileManager(tree, callback);
      }
      break;
      case 1: {
        return Swipepad(_currentDelaySliderValue.toInt(), _sensity.toInt(), callback);
      }
      break;
      case 2: {
        return Trackpad(_currentDelaySliderValue.toInt(), _sensity.toInt(), callback);
      }
      break;
      default: {
        return Center(
          child: Text('Error'),
        );
      }
      break;
    }
  }

  Future<void> initPlatformState() async {
    startListening();
  }

  void onData(ScreenStateEvent event) {
    if (event == ScreenStateEvent.SCREEN_OFF) 
      _sendMessage('q');
    else if (event == ScreenStateEvent.SCREEN_UNLOCKED)
      _sendMessage('o');
    print(event);
  }

  void startListening() {
    _screen = new Screen();
    try {
      _subscription = _screen.screenStateStream.listen(onData);
    } on ScreenStateException catch (exception) {
      print(exception);
    }
  }

  void stopListening() {
    _subscription.cancel();
  }

  // void _delaySliderHandler() {
  //   print('dd');
  // }

  void _onScroll(bool direction) {
    now = DateTime.now();
    if (now.isAfter(lastMessage)) {
      print('Scroll ' + direction.toString());
      _sendMessage(direction?'u':'d');
      lastMessage = now.add(Duration(milliseconds: _currentDelaySliderValue.toInt() * 2));
    }
    if (now.isAfter(lastScroll)) 
      _scaffoldKey.currentState.showSnackBar(
        SnackBar(
          content: Text(direction?'Scroll Up':'Scroll Down'),
          duration: Duration(milliseconds: 500),
      ));
    lastScroll = now.add(Duration(milliseconds: 300));
  }

  void _parseTree(String data) {
    print('parse');
    // print(data);
    buf = 0;
    l = [];
    parsedData = new List< List<String> >();
    for (i = 0; i < data.length; ++i)
      if (data[i] == ' ') {
        if (i - buf > 0)
          l.add(data.substring(buf, i));
      buf = i + 1;
    } else if (data[i] == '/') {
      // l.add(data.substring(buf, i));
      buf = i + 1;
      parsedData.add(l);
      l = [];
    }
    setState(() {
      tree = parsedData;
    });
  }

  void _onDataReceived(Uint8List data) {
    print('New!');
    // Uint8List buffer = Uint8List(data.length);
    // int bufferIndex = buffer.length;
    _messageBuffer += String.fromCharCodes(data);
    print(_messageBuffer);
    if (_messageBuffer.contains('|')) {
      _parseTree(_messageBuffer.substring(0, _messageBuffer.indexOf('|')));
      _messageBuffer = '';
    }
    

    // Allocate buffer for parsed data
    // int backspacesCounter = 0;
    // data.forEach((byte) {
    //   if (byte == 8 || byte == 127) {
    //     backspacesCounter++;
    //   }
    // });
    // Uint8List buffer = Uint8List(data.length - backspacesCounter);
    // int bufferIndex = buffer.length;

    // // Apply backspace control character
    // backspacesCounter = 0;
    // for (int i = data.length - 1; i >= 0; i--) {
    //   if (data[i] == 8 || data[i] == 127) {
    //     backspacesCounter++;
    //   } else {
    //     if (backspacesCounter > 0) {
    //       backspacesCounter--;
    //     } else {
    //       buffer[--bufferIndex] = data[i];
    //     }
    //   }
    // }

    // // Create message if there is new line character
    // String dataString = String.fromCharCodes(buffer);
    // _parseTree(dataString);
    // int index = buffer.indexOf(13);
    // if (~index != 0) {
    //   _parseTree(
    //     backspacesCounter > 0
    //         ? _messageBuffer.substring(
    //             0, _messageBuffer.length - backspacesCounter)
    //         : _messageBuffer + dataString.substring(0, index),
    //   );
    //   _messageBuffer = dataString.substring(index);
    // } else {
    //   _messageBuffer = (backspacesCounter > 0
    //       ? _messageBuffer.substring(
    //           0, _messageBuffer.length - backspacesCounter)
    //       : _messageBuffer + dataString);
    // }
  }

  void _sendTime() {
    now = DateTime.now();
    _sendMessage((now.hour*60 + now.minute).toString() + 'c');
  }

  void _sendMessage(String text) async {
    print(text);
    try {
        connection.output.add(utf8.encode(text + "\r\n"));
        await connection.output.allSent;
    } catch (e) {
      print(e);
    }
  }
}
