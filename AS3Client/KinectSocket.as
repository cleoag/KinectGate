package {
	import flash.display.Graphics;
	import flash.display.Sprite;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.geom.Point;
	import flash.net.Socket;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.setTimeout;

	/**
	 * @author den.ivanov
	 */
	public class KinectSocket extends Sprite {
		private var socket : Socket;
		private var bw : ByteArray;
		private var skeletonsCount : int = 0;
		private var skeletons : Vector.<Skeleton>;
		private var centralSkeleton : int = -1;

		public function KinectSocket() {
			skeletons = new Vector.<Skeleton>(Skeleton.NUI_SKELETON_COUNT, true);

			for (var i : int = 0;i < Skeleton.NUI_SKELETON_COUNT;i++) {
				skeletons[i] = new Skeleton(i);
			}

			bw = new ByteArray();
			bw.length = 100;

			socket = new Socket();
			socket.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			socket.addEventListener(ProgressEvent.SOCKET_DATA, getData);
			connectToSocket();
		}

		private function connectToSocket() : void {
			trace("connect to socket server");
			socket.connect("localhost", 6001);
			socket.writeByte(1);
			socket.flush();
		}

		private function ioErrorHandler(event : IOErrorEvent) : void {
			trace("error connecting to socket server");
			setTimeout(connectToSocket, 3000);
		}

		public function getPoint() : Point {
			if (centralSkeleton == -1) {
				return new Point(-1, -1);
			} else {
				return new Point(skeletons[centralSkeleton].joints[11].x,skeletons[centralSkeleton].joints[11].y);
			}
		}

		private function getData(event : ProgressEvent) : void {
			var g : Graphics = this.graphics;
			//g.clear();
			//g.beginFill(0x000000, 0.1);
			//g.drawRect(0, 0, 640, 480);
			//g.endFill();
			//trace("data", socket.bytesAvailable);
			if (socket.bytesAvailable > 0) {
				socket.readBytes(bw, 0, socket.bytesAvailable);
				bw.position = 0;
				bw.endian = Endian.LITTLE_ENDIAN;
				// parse data
				skeletonsCount = bw.readFloat();

				if (skeletonsCount == 0) {
					centralSkeleton = -1;
				} else {
					for (var n : int = 0;n < 6;n++) {
						skeletons[skeletonID].reset();
					}

					var isCentralSkeletonStillHere : Boolean = false;
					var newCentralSkeleton : int = -1;
					var centerX : Number = 800/2;

					for (var i : int = 0;i < skeletonsCount;i++) {
						var skeletonID : int = bw.readFloat();
						if (skeletonID == centralSkeleton) {
							isCentralSkeletonStillHere = true;
						}
						for (var j : int = 0;j < Skeleton.NUI_SKELETON_POSITION_COUNT;j++) {
							var jointX : Number = bw.readFloat();
							var jointY : Number = bw.readFloat();

							skeletons[skeletonID].updateJoint(j, Math.round(jointX*800), Math.round(jointY*600));
						}
						// test for new central
						var dist : Number = Math.abs(skeletons[skeletonID].joints[0].x - 800/2);
						if (dist < centerX) {
							centerX = dist;
							newCentralSkeleton = skeletonID;
						}
						// draw skeleton
						 skeletons[skeletonID].draw(this);
					}
					if (!isCentralSkeletonStillHere) {
						centralSkeleton = newCentralSkeleton;
					}
				}

				socket.writeByte(1);
				socket.flush();
			}
		}
	}
}
