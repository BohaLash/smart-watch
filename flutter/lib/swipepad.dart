import 'package:flutter/material.dart';
import 'dart:async';
import 'dart:math';

class Swipepad extends StatefulWidget {
	int timing, speed;
	Function(String) callback;

	Swipepad(this.timing, this.speed, this.callback);

  @override
  _SwipepadState createState() => _SwipepadState();
}

class _SwipepadState extends State<Swipepad> {
	Offset a;
	int distance = 0;
	var last = DateTime.now();
	var now;
	
	int _getDistance(Offset b) {
		return sqrt(pow((b.dx - a.dx), 2) + pow((b.dy - a.dy), 2)).toInt();
	}

  @override
  Widget build(BuildContext context) {
    return Listener(
    	onPointerDown: (PointerEvent details) {
    		a = details.position;
  		},
      onPointerUp: (PointerEvent details) {
        distance = _getDistance(details.position);
      	now = DateTime.now();
      	if (distance > 10) {
          if ((details.position.dx - a.dx).abs() > (details.position.dy - a.dy).abs())
            widget.callback(((details.position.dx - a.dx)*widget.speed*0.15).round().toString() + 'i');
          else
            widget.callback(((details.position.dy - a.dy)*widget.speed*0.15).round().toString() + 'j');
      	} else if (now.isAfter(last)) {
          print('t');
          widget.callback('t');
          last = now.add(Duration(milliseconds: widget.timing * 10 + 100));
      	}
      },
      child: Container(
        height: double.infinity,
        width: double.infinity,
        child: Center(
        	child: Text(
        		'Swipe me!',
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
