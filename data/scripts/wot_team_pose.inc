@ ============================================================================
@  WoT: the team pose. L in the field (outdoors or in a cave, on foot or on a
@  bike) brings the whole party out of their balls into a V behind the player,
@  facing the camera like a team photo; any button sends them back.
@  Started from ProcessPlayerFieldInput; the work is in src/wot_team_pose.c.
@ ============================================================================
EventScript_WotTeamPose::
	lockall
	special WotTeamPose_Plan
	goto_if_eq VAR_RESULT, 0, EventScript_WotTeamPoseNoRoom
	hidefollower
	special WotTeamPose_Run
	releaseall
	end

EventScript_WotTeamPoseNoRoom:
	msgbox Text_WotTeamPoseNoRoom, MSGBOX_DEFAULT
	releaseall
	end

Text_WotTeamPoseNoRoom:
	.string "There isn't enough room here\n"
	.string "for your team to pose.$"
