package {
	import assets.PointerAsset;

	import flash.display.DisplayObject;
	import flash.display.Graphics;
	import flash.events.Event;
	import flash.geom.Point;

	/**
	 * @author den.ivanov
	 */
	public class Pointer extends PointerAsset {
		private var zones : Array;
		public var currentZone : DisplayObject;
		public var previousZone : DisplayObject;
		private var zoneTime : int = 0;
		private var maxZoneTime : Number = 30;

		public function Pointer() {
			zones = new Array();
		}

		public function addZone(d : DisplayObject) : void {
			zones.push(d);
		}

		public function check(p : Point) : void {
			var isOverZone : Boolean = false;

			for (var i : int = 0;i < zones.length;i++) {
				var checkZone : DisplayObject = DisplayObject(zones[i]);
				if (checkZone.hitTestPoint(p.x, p.y, true)) {
					isOverZone = true;
					if (currentZone == checkZone && checkZone != previousZone) {
						zoneTime++;
						if (zoneTime >= maxZoneTime) {
							previousZone = currentZone;
							zoneTime = 0;
							currentZone.dispatchEvent(new Event(Event.CHANGE));
						}
					} else {
						zoneTime = 0;
						currentZone = checkZone;
					}
				}
			}
			if (isOverZone == false) {
				currentZone = null;
				previousZone = null;
				zoneTime = 0;
			}

			var g : Graphics = this.graphics;
			g.clear();
			g.lineStyle(10, 0x004a9f, 1);
			var aStep : Number = Math.PI * 2 / maxZoneTime;
			for (i = 0;i < zoneTime;i++) {
				var px : Number = 45 * Math.cos(i * aStep);
				var py : Number = 45 * Math.sin(i * aStep);
				if (i == 0) {
					g.moveTo(px, py);
				} else {
					g.lineTo(px, py);
				}
			}
		}
	}
}
