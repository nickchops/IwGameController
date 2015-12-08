--[[/*
 * (C) 2012-2013 Marmalade.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */--]]

--[[

/**
 Global unified analytics API that supports all platforms:
*/

--]]

--------------------------------------------------------------------------------
-- Public API
--------------------------------------------------------------------------------
analytics = {}

--[[
/**
@brief Checks if analytics is ready to use.

@return True if analytics is ready, false otherwise.
*/
--]]
function analytics:isReady()
	return quick.QAnalytics:isReady()
end

--[[
/**
 * @brief   Creates and initialises the analytics system.
 *
 * Creates and initialises the analytics system, calling the supplied ready_callback when
 * the analytics system has finished initialising. 
 *
 * Once the analytics system has finished initialising you should send a start session event
 * then any analytics events that you wish to track. When the session is over or when the device is
 * suspended send the session end event. When the device resumes from a suspended state you should
 * send a new session start event.
 *
 * @param   game_key  The game key provided by Game Analytics
 * @param   secret key   The secret key provided by Game Analytics
 * @param   build_version   The build version of your game
 *
 * @return  True if it succeeds, False if it fails.
 */
--]]
function analytics:init(game_key, secret_key, version)
    dbg.assertFuncVarType("string", game_key)
    dbg.assertFuncVarType("string", secret_key)
    dbg.assertFuncVarType("string", version)
	return quick.QAnalytics:init(game_key, secret_key, version)
end

--[[
/**
 * @brief   Terminates the analytics system.
 */
--]]
function analytics:terminate()
	quick.QAnalytics:terminate()
end

--[[
/**
 * @brief   Updates the analytics system, should be called every frame
 */
--]]
function analytics:update()
	quick.QAnalytics:update()
end

--[[
/**
 * @brief   Sets the session ID, overriding the default randomly generated ID.
 *
 * @param   Session ID string
 */
--]]
function analytics:setSessionID(currency)
    dbg.assertFuncVarType("string", session_id)
	quick.QAnalytics:setSessionID(session_id)
end

--[[
/**
 * @brief   Sets the currency used by in app purchase analytics, e.g. USD.
 *
 * @param   currency  The billing currency string, e.g. USD
 */
--]]
function analytics:setCurrency(currency)
    dbg.assertFuncVarType("string", currency)
	quick.QAnalytics:setCurrency(currency)
end

--[[
/**
 * @brief   Sets the gender of the user
 *
 * @param   game_key  Users gender (male or female)
 */
--]]
function analytics:setGender(gender)
    dbg.assertFuncVarType("string", gender)
	quick.QAnalytics:setGender(gender)
end

--[[
/**
 * @brief   Sets the birth year of the user
 *
 * @param   year  Birth year of the user
 */
--]]
function analytics:setBirthYear(year)
    dbg.assertFuncVarType("number", year)
	quick.QAnalytics:setBirthYear(year)
end

--[[
/**
 * @brief   Sets the 1st custom dimension for all consecutive events
 *
 * @param   dimension  Dimension string.
 */
--]]
function analytics:setCustom1(dimension)
    dbg.assertFuncVarType("string", dimension)
	quick.QAnalytics:setCustom1(dimension)
end

--[[
/**
 * @brief   Sets the 2nd custom dimension for all consecutive events
 *
 * @param   dimension  Dimension string.
 */
--]]
function analytics:setCustom2(dimension)
    dbg.assertFuncVarType("string", dimension)
	quick.QAnalytics:setCustom2(dimension)
end

--[[
/**
 * @brief   Sets the 3rd custom dimension for all consecutive events
 *
 * @param   dimension  Dimension string.
 */
--]]
function analytics:setCustom3(dimension)
    dbg.assertFuncVarType("string", dimension)
	quick.QAnalytics:setCustom3(dimension)
end

--[[
/**
 * @brief   Marks the start of a collection of events that should be batched and then later sent together.
 *
 * @return  True if it succeeds, False if it fails.
 */
--]]
function analytics:startBatch()
	return quick.QAnalytics:startBatch()
end

--[[
/**
 * @brief   Sends all events that were batched since the call to startBatch().
 */
--]]
function analytics:sendBatch()
	quick.QAnalytics:sendBatch()
end

--[[
/**
 * @brief   Sets the 3rd custom dimension for all consecutive events
 *
 * @param   item_type       Type of the item.
 * @param   item_id         Identifier for the item.
 * @param   currency        The currency.
 * @param   amount          The amount.
 * @param   cart_type       Type of the cart.
 * @param   receipt_info    The app store receipt.
 */
--]]
function analytics:sendBusinessEvent(item_type, item_id, currency, amount, cart_type, receipt_info)
    dbg.assertFuncVarType("string", item_type)
    dbg.assertFuncVarType("string", item_id)
    dbg.assertFuncVarType("string", currency)
    dbg.assertFuncVarType("number", amount)
    dbg.assertFuncVarType("string", cart_type)
    dbg.assertFuncVarType("string", receipt_info)
	quick.QAnalytics:sendBusinessEvent(item_type, item_id, currency, amount, cart_type, receipt_info)
end

--[[
/**
 * @brief   Sends a resource analytics event, resource events are used to track resource acquisition and consumption, e.g.:
 *
 * @param   flow_type   Flow type string, can be sink or source
 * @param   currency    The currency.
 * @param   item_type   Type of the item.
 * @param   item_id     Identifier for the item.
 * @param   amount      The amount.
 */
--]]
function analytics:sendResourceEvent(flow_type, currency, item_type, item_id, amount)
    dbg.assertFuncVarType("string", flow_type)
    dbg.assertFuncVarType("string", currency)
    dbg.assertFuncVarType("string", item_type)
    dbg.assertFuncVarType("string", item_id)
    dbg.assertFuncVarType("number", amount)
	quick.QAnalytics:sendResourceEvent(flow_type, currency, item_type, item_id, amount)
end

--[[
/**
 * @brief   Sends a progression analytics event, progress events are used to track the users progresses through the game, e.g.:
 *
 * @param   status          Progression status, can be start, fail or complete
 * @param   progression1    The first progression property.
 * @param   progression2    The second progression property.
 * @param   progression3    The third progression property.
 * @param   attempt_run     The attempt run.
 * @param   score           The score.
 */
--]]
function analytics:sendProgressionEvent(status, progression1, progression2, progression3, attempt_run, score)
    dbg.assertFuncVarType("string", status)
    dbg.assertFuncVarType("string", progression1)
    dbg.assertFuncVarType("string", progression2)
    dbg.assertFuncVarType("string", progression3)
    dbg.assertFuncVarType("number", attempt_run)
    dbg.assertFuncVarType("number", score)
	quick.QAnalytics:sendProgressionEvent(status, progression1, progression2, progression3, attempt_run, score)
end

--[[
/**
 * @brief   Sends a design analytics event, design events are used to track events that re.late to game play.
 *
 * @param   event_id    Identifier for the event.
 * @param   value       A value to pass with the event.
 */
--]]
function analytics:sendDesignEvent(event_id, value)
    dbg.assertFuncVarType("string", event_id)
    dbg.assertFuncVarType("number", value)
	quick.QAnalytics:sendDesignEvent(event_id, value)
end

--[[
/**
 * @brief   Sends an error analytics event.
 *
 * @param   error_severity  The errors severity, can be debug, info, warning, error or critical
 * @param   message         The message to send with the error.
 */
--]]
function analytics:sendErrorEvent(error_severity, message)
    dbg.assertFuncVarType("string", error_severity)
    dbg.assertFuncVarType("string", message)
	quick.QAnalytics:sendErrorEvent(error_severity, message)
end


--[[
/**
 * @brief   Sends the session start event.
 *          
 * The session start event marks the start of a game session, you should send a session start event when the app 
 * is ran or when resuming from a suspended state.
 *
 */
--]]
function analytics:sendSessionStart()
	quick.QAnalytics:sendSessionStart()
end

--[[
/**
 * @brief   Sends the session end event.
 *
 * The session end event marks the end of a game session, you should send a session end events when the app 
 * is shutting down or when the device is about to be placed in a suspended state.
 *
 */
--]]
function analytics:sendSessionEnd()
	quick.QAnalytics:sendSessionEnd()
end

--[[
/**
 * @brief   Sends a game started event.
 *
 * Sends a game started event, should be sent when the player starts a new game.
 *
 * @param   num_players Number of players in the game.
 */
--]]
function analytics:sendGameStarted(num_players)
    dbg.assertFuncVarType("number", num_players)
	quick.QAnalytics:sendGameStarted(num_players)
end

--[[
/**
 * @brief   Sends the game quit event.
 * 
 * Sends a game quit event, should be sent when a player quits out of a game.
 *
 * @param   position  Position within the game that the player quit, e.g. level1
 */
--]]
function analytics:sendGameQuit(position)
    dbg.assertFuncVarType("string", position)
	quick.QAnalytics:sendGameQuit(position)
end

--[[
/**
 * @brief   Sends the game complete event.
 *
 * Sends a game complete event, should be sent when a player completes the game.
 *
 */
--]]
function analytics:sendGameComplete()
	quick.QAnalytics:sendGameComplete()
end

--[[
/**
 * @brief   Sends a level start event.
 *
 * Sends a level start event, to mark the start of a level.
 *
 * @param   world_name  Name of the world, e.g. world1.
 * @param   stage_name  Name of the stage, e.g. stage5.
 * @param   level_name  Name of the level, e.g. level2.
 */
--]]
function analytics:sendLevelStart(world_name, stage_name, level_name)
    dbg.assertFuncVarType("string", world_name)
    dbg.assertFuncVarType("string", stage_name)
    dbg.assertFuncVarType("string", level_name)
	quick.QAnalytics:sendLevelStart(world_name, stage_name, level_name)
end

--[[
/**
 * @brief   Sends a level complete event.
 *
 * Sends a level complete event, to mark the completion of a level. Should be used in conjunction with SendLevelStart().
 *
 * @param   world_name  Name of the world, e.g. world1.
 * @param   stage_name  Name of the stage, e.g. stage5.
 * @param   level_name  Name of the level, e.g. level2.
 * @param   attempt_run The attempt run count (optional, pass -1 to exclude), e.g. 5
 * @param   score       The players score (optional, pass -1 to exclude), e.g. 1999
 */
--]]
function analytics:sendLevelComplete(world_name, stage_name, level_name, attempt_run, score)
    dbg.assertFuncVarType("string", world_name)
    dbg.assertFuncVarType("string", stage_name)
    dbg.assertFuncVarType("string", level_name)
    dbg.assertFuncVarType("number", attempt_run)
    dbg.assertFuncVarType("number", score)
	quick.QAnalytics:sendLevelComplete(world_name, stage_name, level_name, attempt_run, score)
end

--[[
/**
 * @brief   Sends a level failed event.
 *
 * Sends a level failed event, to mark the failure of a level. Should be used in conjunction with SendLevelStart().
 *
 * @param   world_name  Name of the world, e.g. world1.
 * @param   stage_name  Name of the stage, e.g. stage5.
 * @param   level_name  Name of the level, e.g. level2.
 * @param   score       The players score (optional, pass -1 to exclude), e.g. 1999
 */
--]]
function analytics:sendLevelFailed(world_name, stage_name, level_name, score)
    dbg.assertFuncVarType("string", world_name)
    dbg.assertFuncVarType("string", stage_name)
    dbg.assertFuncVarType("string", level_name)
    dbg.assertFuncVarType("number", score)
	quick.QAnalytics:sendLevelFailed(world_name, stage_name, level_name, score)
end

--[[
/**
 * @brief   Sends a level restart event.
 *
 * @param   level_name  Name of the level.
 */
--]]
function analytics:sendLevelRestart(level_name)
    dbg.assertFuncVarType("string", level_name)
	quick.QAnalytics:sendLevelRestart(level_name)
end

--[[
/**
 * @brief   Sends a level quit event.
 *
 * @param   level_name  Name of the level.
 */
--]]
function analytics:sendLevelQuit(level_name)
    dbg.assertFuncVarType("string", level_name)
	quick.QAnalytics:sendLevelQuit(level_name)
end

--[[
/**
 * @brief   Sends a player died event.
 *
 * @param   reason      The reason for the players death.
 */
--]]
function analytics:sendPlayerDied(reason)
    dbg.assertFuncVarType("string", reason)
	quick.QAnalytics:sendPlayerDied(reason)
end

--[[
/**
 * @brief   Sends a player collected item event.
 *
 * @param   item_name   Name of the item that was collected.
 */
--]]
function analytics:sendPlayerCollected(item_name)
    dbg.assertFuncVarType("string", item_name)
	quick.QAnalytics:sendPlayerCollected(item_name)
end

--[[
/**
 * @brief   Sends an achievement earnt event.
 *
 * @param   achievement_name    Name of the achievement that was earnt.
 */
--]]
function analytics:sendAchievementEarnt(achievement_name)
    dbg.assertFuncVarType("string", achievement_name)
	quick.QAnalytics:sendAchievementEarnt(achievement_name)
end

--[[
/**
 * @brief   Sends the bonus earnt event.
 *
 * @param   bonus_name  Name of the bonus that was earnt.
 */
--]]
function analytics:sendBonusEarnt(bonus_name)
    dbg.assertFuncVarType("string", bonus_name)
	quick.QAnalytics:sendBonusEarnt(bonus_name)
end

--[[
/**
 * @brief   Sends an item purchased event.
 *
 * @param   item_type       Type of the item, e.g. coins.
 * @param   item_id         Identifier for the item, e.g. coins10.
 * @param   amount          The amount in real currency, e.g. 99 (99 cents).
 * @param   cart_type       Cart type (optional).
 * @param   receipt_info    Receipt information (optional).
 */
--]]
function analytics:sendItemPurchased(item_type, item_id, amount, cart_type, receipt_info)
    dbg.assertFuncVarType("string", item_type)
    dbg.assertFuncVarType("string", item_id)
    dbg.assertFuncVarType("number", amount)
    dbg.assertFuncVarType("string", cart_type)
    dbg.assertFuncVarType("string", receipt_info)
	quick.QAnalytics:sendItemPurchased(item_type, item_id, amount, cart_type, receipt_info)
end

--[[
/**
 * @brief   Sends a resource gained event.
 *
 * @param   resource_type       Type of resource, e.g. oak plank.
 * @param   resource_id         Identifier for the resource, e.g. planka.
 * @param   resource_currency   The resource currency, e.g. wood.
 * @param   amount              The amount gained, e.g 10.
 */
--]]
function analytics:sendResourceGained(resource_type, resource_id, resource_currency, amount)
    dbg.assertFuncVarType("string", resource_type)
    dbg.assertFuncVarType("string", resource_id)
    dbg.assertFuncVarType("string", resource_currency)
    dbg.assertFuncVarType("number", amount)
	quick.QAnalytics:sendResourceGained(resource_type, resource_id, resource_currency, amount)
end

--[[
/**
 * @brief   Sends a resource used event.
 *
 * @param   resource_type       Type of resource.
 * @param   resource_id         Identifier for the resource, e.g. planka.
 * @param   resource_currency   The resource currency, e.g. wood.
 * @param   amount              The amount used, e.g 10.
 */
--]]
function analytics:sendResourceUsed(resource_type, resource_id, resource_currency, amount)
    dbg.assertFuncVarType("string", resource_type)
    dbg.assertFuncVarType("string", resource_id)
    dbg.assertFuncVarType("string", resource_currency)
    dbg.assertFuncVarType("number", amount)
	quick.QAnalytics:sendResourceUsed(resource_type, resource_id, resource_currency, amount)
end

--[[
/**
 * @brief   Sends an ad shown event.
 *
 * @param   ad_name Name of the ad.
 */
--]]
function analytics:sendAdShown(ad_name)
    dbg.assertFuncVarType("string", ad_name)
	quick.QAnalytics:sendAdShown(ad_name)
end

--[[
/**
 * @brief   Sends an ad clicked event.
 *
 * @param   ad_name Name of the ad.
 */
--]]
function analytics:sendAdClicked(ad_name)
    dbg.assertFuncVarType("string", ad_name)
	quick.QAnalytics:sendAdClicked(ad_name)
end

--[[
/**
 * @brief   Sends an error event.
 *
 * @param   message The error message.
 */
--]]
function analytics:sendError(message)
    dbg.assertFuncVarType("string", message)
	quick.QAnalytics:sendError(message)
end

--[[
/**
 * @brief   Sends a warning event.
 *
 * @param   message The warning message.
 */
--]]
function analytics:sendWarning(message)
    dbg.assertFuncVarType("string", message)
	quick.QAnalytics:sendWarning(message)
end

--[[
/**
 * @brief   Sends an informational event.
 *
 * @param   message The informational message.
 */
--]]
function analytics:sendInfo(message)
    dbg.assertFuncVarType("string", message)
	quick.QAnalytics:sendInfo(message)
end

--[[
/**
 * @brief   Sends a debug event.
 *
 * @param   message The debug message.
 */
--]]
function analytics:sendDebug(message)
    dbg.assertFuncVarType("string", message)
	quick.QAnalytics:sendDebug(message)
end

