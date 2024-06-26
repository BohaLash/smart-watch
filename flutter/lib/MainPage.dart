import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:scoped_model/scoped_model.dart';

import './ChatPage.dart';
import './SelectBondedDevicePage.dart';
import './BackgroundCollectingTask.dart';

class MainPage extends StatefulWidget {
  @override
  _MainPage createState() => new _MainPage();
}

class _MainPage extends State<MainPage> {
  BluetoothState _bluetoothState = BluetoothState.UNKNOWN;

  String _address = "...";
  String _name = "...";

  Timer _discoverableTimeoutTimer;
  int _discoverableTimeoutSecondsLeft = 0;

  BackgroundCollectingTask _collectingTask;

  bool _autoAcceptPairingRequests = false;

  callback(BluetoothDevice selectedDevice) {
    _startChat(context, selectedDevice);
  }

  @override
  void initState() {
    super.initState();

    // Get current state
    FlutterBluetoothSerial.instance.state.then((state) {
      setState(() {
        _bluetoothState = state;
      });
    });

    Future.doWhile(() async {
      // Wait if adapter not enabled
      if (await FlutterBluetoothSerial.instance.isEnabled) {
        return false;
      }
      await Future.delayed(Duration(milliseconds: 0xDD));
      return true;
    }).then((_) {
      // Update the address field
      FlutterBluetoothSerial.instance.address.then((address) {
        setState(() {
          _address = address;
        });
      });
    });

    FlutterBluetoothSerial.instance.name.then((name) {
      setState(() {
        _name = name;
      });
    });

    // Listen for futher state changes
    FlutterBluetoothSerial.instance
        .onStateChanged()
        .listen((BluetoothState state) {
      setState(() {
        _bluetoothState = state;

        // Discoverable mode is disabled when Bluetooth gets disabled
        _discoverableTimeoutTimer = null;
        _discoverableTimeoutSecondsLeft = 0;
      });
    });
  }

  @override
  void dispose() {
    FlutterBluetoothSerial.instance.setPairingRequestHandler(null);
    _collectingTask?.dispose();
    _discoverableTimeoutTimer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Hack Watch App'),
      ),
      body: Container(
        child: ListView(
          children: <Widget>[
            SizedBox(
              height: 20.0,
            ),
            ListTile(
              title: const Text(
                'Setup:',
                textScaleFactor: 2,
                style: TextStyle(
                  fontWeight: FontWeight.bold,
                ),
              ),
              // contentPadding: EdgeInsets.only(
              //   top: 20.0,
              //   left: 15.0,
              // ),
            ),
            SwitchListTile(
              title: const Text('Enable Bluetooth'),
              value: _bluetoothState.isEnabled,
              onChanged: (bool value) {
                // Do the request and update with the true value then
                future() async {
                  // async lambda seems to not working
                  if (value)
                    await FlutterBluetoothSerial.instance.requestEnable();
                  else
                    await FlutterBluetoothSerial.instance.requestDisable();
                }

                future().then((_) {
                  setState(() {});
                });
              },
            ),
            ListTile(
              title: const Text('Bluetooth status'),
              subtitle: Text(_bluetoothState.toString()),
              trailing: RaisedButton(
                child: const Text('Settings'),
                onPressed: () {
                  FlutterBluetoothSerial.instance.openSettings();
                },
              ),
            ),
            // Divider(),
            SizedBox(
              height: 20.0,
            ),
            ListTile(
              title: const Text(
                'Connect to Device:',
                textScaleFactor: 2,
                style: TextStyle(
                  fontWeight: FontWeight.bold,
                ),
              )
            ),
            SelectBondedDevicePage(checkAvailability: false, callback: callback),
            // Container(
            //   height: 500.0,
            //   child: SelectBondedDevicePage(checkAvailability: false),
            // ),
            // ListTile(
            //   title: RaisedButton(
            //     child: const Text('Connect to paired device'),
            //     onPressed: () async {
            //       final BluetoothDevice selectedDevice =
            //           await Navigator.of(context).push(
            //         MaterialPageRoute(
            //           builder: (context) {
            //             return SelectBondedDevicePage(checkAvailability: false);
            //           },
            //         ),
            //       );

            //       if (selectedDevice != null) {
            //         print('Connect -> selected ' + selectedDevice.address);
            //         _startChat(context, selectedDevice);
            //       } else {
            //         print('Connect -> no device selected');
            //       }
            //     },
            //   ),
            // ),
          ],
        ),
      ),
    );
  }

  void _startChat(BuildContext context, BluetoothDevice server) {
    Navigator.of(context).push(
      MaterialPageRoute(
        builder: (context) {
          return ChatPage(server: server);
        },
      ),
    );
  }

  Future<void> _startBackgroundTask(
    BuildContext context,
    BluetoothDevice server,
  ) async {
    try {
      _collectingTask = await BackgroundCollectingTask.connect(server);
      await _collectingTask.start();
    } catch (ex) {
      if (_collectingTask != null) {
        _collectingTask.cancel();
      }
      showDialog(
        context: context,
        builder: (BuildContext context) {
          return AlertDialog(
            title: const Text('Error occured while connecting'),
            content: Text("${ex.toString()}"),
            actions: <Widget>[
              new FlatButton(
                child: new Text("Close"),
                onPressed: () {
                  Navigator.of(context).pop();
                },
              ),
            ],
          );
        },
      );
    }
  }
}
