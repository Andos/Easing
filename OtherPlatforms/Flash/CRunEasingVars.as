package Extensions 
{
	import flash.utils.Timer;
	/**
	 * ...
	 * @author Anders Riggelsen
	 */
	public class CRunEasingVars 
	{
		public var overshoot:Number;
		public var amplitude:Number;
		public var period:Number;
		
		public var startX:int;
		public var startY:int;
		public var destX:int;
		public var destY:int;
	
		public var easingMode:int;
		public var functionA:int;
		public var functionB:int;

		public var timeMode:int;
		public var starttime:int;
		public var startdate:Date;
		
		public var timespan:int;
		public var eventloop_step:int;
		public var fixed:int;
		
		public function CRunEasingVars() 
		{
		}
	}
}