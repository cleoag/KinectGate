package {
	import flash.display.Graphics;
	import flash.display.Sprite;
	import flash.geom.Point;

	/**
	 * @author den.ivanov
	 */
	public class Skeleton {
		public var isActive : Boolean = false;
		public var id : int = -1;
		public var joints : Vector.<Point>;
		public static const NUI_SKELETON_POSITION_COUNT : int = 20;
		public static const NUI_SKELETON_COUNT : int = 6;

		public function Skeleton(_id : int = -1) {
			joints = new Vector.<Point>(NUI_SKELETON_POSITION_COUNT, true);

			isActive = false;
			id = _id;
			for (var i : int = 0;i < NUI_SKELETON_POSITION_COUNT;i++) {
				joints[i] = new Point(0, 0);
			}
		}

		public function updateJoint(_index : int, _x : Number, _y : Number) : void {
			joints[_index].x = _x;
			joints[_index].y = _y;
		}

		public function draw(s : Sprite, _color : int = 0xff0000) : void {
			var g : Graphics = s.graphics;

			var size : Number = 10;
			
			var colors:Array = [0xff0000,0xff0000,0xff0000,0x00ff00,0xff0000,0xff0000,0xff0000,0xffff00,0xff0000,0xff0000,0xff0000,0x0000ff,0xff0000,0xff0000,0xff0000,0xff0000,0xff0000,0xff0000,0xff0000,0xff0000];

			for (var i : int = 0;i < NUI_SKELETON_POSITION_COUNT;i++) {
				g.beginFill(colors[i], 1);
				g.drawRect(joints[i].x - size / 2, joints[i].y - size / 2, size, size);
				g.endFill();
			}
		}

		public function reset() : void {
			isActive = false;
			for (var i : int = 0;i < NUI_SKELETON_POSITION_COUNT;i++) {
				joints[i] = new Point(0, 0);
			}
		}
	}
}
