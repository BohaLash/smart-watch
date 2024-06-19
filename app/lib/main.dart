import 'package:flutter/material.dart';
import 'package:flutter/services.dart' ;

import './MainPage.dart';

void main() => runApp(MyApplication());

class MyApplication extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
  	SystemChrome.setPreferredOrientations([
		DeviceOrientation.portraitUp,
		DeviceOrientation.portraitDown,
	]);
    return MaterialApp(home: MainPage());
  }
}
