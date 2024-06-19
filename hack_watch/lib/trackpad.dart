import 'package:flutter/material.dart';
import 'dart:async';
import 'dart:math';

class Trackpad extends StatefulWidget {
	int timing, speed;
	Function(String) callback;

	Trackpad(this.timing, this.speed, this.callback);

  @override
  _TrackpadState createState() => _TrackpadState();
}

class _TrackpadState extends State<Trackpad> {
	Offset a;
	int distance = 0;
	var last = DateTime.now();
	var now;
	bool b = false;
	
	int _getDistance(Offset b) {
		return sqrt(pow((b.dx - a.dx), 2) + pow((b.dy - a.dy), 2)).toInt();
	}

  @override
  Widget build(BuildContext context) {
    return Listener(
    	onPointerDown: (PointerEvent details) {
    		a = details.position;
    		// last = DateTime.now();
    		b = true;
    		// distance = 0;
  		},
      onPointerMove: (PointerEvent details) {
      	distance = _getDistance(details.position);
      	now = DateTime.now();
      	if (distance > 10 && now.isAfter(last)) {
      		b = false;
      		// cos = x / distance
										print((details.position.dx - a.dx).round().toString() + ' ' + (details.position.dy - a.dy).round().toString());
					widget.callback(((details.position.dx - a.dx)*widget.speed*0.07).round().toString() + 'x\n' + ((details.position.dy - a.dy)*widget.speed*0.07).round().toString() + 'y');
      		a = details.position;
      		last = now.add(Duration(milliseconds: widget.timing * 10 + 30));
				}
      },
      onPointerUp: (PointerEvent details) {
      	now = DateTime.now();
      	if (b && now.isAfter(last)) {
      		print('t');
      		widget.callback('t');
      		last = now.add(Duration(milliseconds: widget.timing * 10 + 100));
      	} else if (distance > 10) {
										print((details.position.dx - a.dx).round().toString() + ' ' + (details.position.dy - a.dy).round().toString());
					widget.callback((details.position.dx - a.dx).round().toString() + 'x\n' + (details.position.dy - a.dy).round().toString() + 'y');
      		a = details.position;
      		last = now.add(Duration(milliseconds: widget.timing * 10 + 30));
      	}
      },
      child: Container(
        height: double.infinity,
        width: double.infinity,
        child: Center(
        	child: Text(
        		'Touch me!',
        		style: TextStyle(
              color: Colors.grey,
              fontWeight: FontWeight.bold,
            ),
					),
      	),
      	color: Colors.black,
      ),
    );
  }
}


				// _timer = new Timer(Duration(seconds: 3), () {
				//   print(details.position);
				// 	widget.callback('10');
				// });
//details.position.dx.toString() + ' ' + details.position.dy.toString()
// milliseconds: 100

// @override
// Widget build(BuildContext context) {
//   return GestureDetector(
//     onTapDown: (TapDownDetails details) => print('${details.globalPosition}'),
//     // onPanUpdate: (PointerEvent details) => print('${details.globalPosition}'),
//     child: Container(
//       height: double.infinity,
//       width: double.infinity,
//       child: Text('Touch me!'),
//     ),
//   );
// }
