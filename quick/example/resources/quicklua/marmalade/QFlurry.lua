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
 Global Flurry analytics API that supports the following platforms:
 - Android
 - iOS

 Function descriptions:
 - flurry:isAvailable() - Returns true if flurry is supported by the platform
 - flurry:startSession(apiKey) - Starts a flurry analytics session with the supplied API KEY
 - flurry:logEvent(name, params) - Logs the named event, with optional table of parameters (key, value strings). Returns 
 true if successful or false otherwise
 - flurry:logError(name, message) - Logs the named error with supplied message

 Limitations:
 - Maximum logEvent parameter key length is 255 characters
 - Maximum logEvent parameter value length is 255 characters
 - Maximum logEvent parameters is 100
 
 Example:

 if (flurry:isAvailable()) then
	flurry:startSession("YOUR_API_KEY_GOES_HERE")
	flurry:logEvent("Game Started")
	flurry:logEvent("Game Menu", {option="Option1", data="Selected", time="18:10"})
 else
	dbg.log("Flurry not available")
 end

*/

--]]

--------------------------------------------------------------------------------
-- Public API
--------------------------------------------------------------------------------
flurry = {}

--[[
/**
@brief Checks availability of Flurry analytics.

@return True if Flurry analytics is available, false otherwise.
*/
--]]
function flurry:isAvailable()
	return quick.QFlurry:isAvailable()
end

--[[
/**
@brief Initialises a new session.

@param apiKey The applications Flurry API key.
*/
--]]
function flurry:startSession(apiKey)
    dbg.assertFuncVarType("string", apiKey)
	return quick.QFlurry:startSession(apiKey)
end

--[[
/**
@brief Ends the current session.

*/
--]]
function flurry:endSession()
	return quick.QFlurry:endSession()
end

--[[
/**
@brief Logs an event.

@param name The name of the event.
@param params A table of key value pairs (must both be strings) that will be logged with the event (optional)
*/
--]]
function flurry:logEvent(name, params)
    dbg.assertFuncVarType("string", name)

	if (params ~= nil) then
	    dbg.assertFuncVarType("table", params)
		quick.QFlurry:_clearParams()
        for i,v in pairs(params) do
            if type(v) == "string" then
                quick.QFlurry:_addParam(i, v)
            end
        end
		quick.QFlurry:logEventParams(name)
	else
		quick.QFlurry:logEvent(name)
	end
end

--[[
/**
@brief Logs an error.

@param name The name of the error.
@param mmessage An additional error message that will be logged with the error.
*/
--]]
function flurry:logError(name, message)
    dbg.assertFuncVarType("string", name)
    dbg.assertFuncVarType("string", message)

	quick.QFlurry:logError(name, message)
end


