package {
	import flash.display.BlendMode;
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.filesystem.File;
	import flash.geom.Point;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.ui.Mouse;

	/**

	 * @author den.ivanov
	 */
	[SWF(backgroundColor="#FFFFFF", frameRate="30", width="800", height="600")]
	public class Main extends Sprite {
		private var socket : KinectSocket;
		private var pointer : Pointer;

		public function CornerPagesAIR() {

			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;

			Mouse.hide();

			
			// object controlled by right hand
			pointer = new Pointer();
			addChild(pointer);

			
			// Add any Display object for hittesting with Pointer
			//pointer.addZone(cb);

			socket = new KinectSocket();
			addChild(socket);

			addEventListener(Event.ENTER_FRAME, updateFrame);

		}

		private function updateFrame(e : Event) : void {
			var newPoint : Point = getPointer();

			pointer.x = newPoint.x;
			pointer.y = newPoint.y;

			pointer.check(newPoint);

		}

		private function getPointer() : Point {
			// just for testing
			// var p:Point = new Point(stage.mouseX, stage.mouseY);
			
			var p : Point = socket.getPoint();
			return p;
		}
	}
}
