
class RedneckLumberBlade : DukeActor
{
	default
	{
		spriteset "LUMBERBLADE1", "LUMBERBLADE";
	}
	
	override void Initialize(DukeActor spawner)
	{
		self.ChangeStat(STAT_LUMBERMILL);
		self.extra = 0;
	}

	override void Tick()
	{
		if (self.hitag == 100)
		{
			self.pos.Z += 4;
			if (self.pos.Z >= self.sector.floorz + 59.25)
				self.pos.Z = self.sector.floorz + 59.25;
		}

		if (self.spritesetindex == 1)
		{
			self.extra++;
			if (self.extra == 192)
			{
				self.hitag = 0;
				self.pos.Z = self.sector.floorz - 59.25;
				self.extra = 0;
				self.setSpritesetImage(0);
				dlevel.resetswitch(999);
			}
		}
	}

	override bool onUse(DukePlayer user)
	{
		if (user == null) // guard against unwanted activation
		{
			self.setSpritesetImage(1);
			self.hitag = 100;
			self.extra = 0;
			self.PlayActorSound("BK_JIB2");
			return true;
		}
		return false;
	}

	override void onTouch(DukePlayer user)
	{
		if (self.spritesetindex == 0)
		{
			user.quickkill();
			user.actor.PlayActorSound("JOE9000B");
		}
	}
}


// Development garbage?
Class RedneckKegHolder : DukeActor
{
	default
	{
		pic "KEGHOLDER";
	}
	
	override void Initialize(DukeActor spawner)
	{
		self.cstat = CSTAT_SPRITE_INVISIBLE;
		self.ChangeStat(STAT_LUMBERMILL);
	}
	
	override bool OnUse(DukePlayer user)
	{
		if (user != null) return false;
		self.Destroy();
		return true;
	}
		
}