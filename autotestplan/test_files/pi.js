/// <reference path="../../../ProjectInsight.TypeScript/app/js/PI.d.ts" />
var __extends = (this && this.__extends) || function (d, b)
{
	for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
	function __() { this.constructor = d; }
	d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
};
/**
 * The PI Module that contains all Project Insight related functionality.
 * The purpose for this library is to have a set of standard exceptions that can be thrown
 * for the purposes of programmer related problems.
 */
var pi;

(function (pi)
{
	/**
     * Base class for Exceptions.
     */
	var Exception = (function ()
	{
		/** Exception constructor. */
		function Exception(message, innerException)
		{
			this.message = message;
			if (innerException)
			{
				this.innerException = innerException;
			}
			else
			{
				this.innerException = null;
			}
		}
		Object.defineProperty(Exception.prototype, "Message", {
			/** Get the Message of the exception. */
			get: function ()
			{
				return this.message;
			},
			enumerable: true,
			configurable: true
		});
		Object.defineProperty(Exception.prototype, "InnerException", {
			/** Get the inner Exception */
			get: function ()
			{
				return this.innerException;
			},
			enumerable: true,
			configurable: true
		});
		return Exception;
	}());
	pi.Exception = Exception;
})(pi || (pi = {}));

/**
 * The PI Module that contains all Project Insight related functionality.
 * The purpose for this library is to have a set of standard exceptions that can be thrown
 * for the purposes of programmer related problems.
 */

(function (pi)
{
	/**
     * Exception used when an argument supplied to a function is invalid.
     */
	var InvalidArgumentException = (function (_super)
	{
		__extends(InvalidArgumentException, _super);
		function InvalidArgumentException(message, innerException)
		{
			_super.call(this, message, innerException);
		}
		return InvalidArgumentException;
	}(pi.Exception));
	pi.InvalidArgumentException = InvalidArgumentException;
	;
})(pi || (pi = {}));
/**
 * The PI Module that contains all Project Insight related functionality.
 * The purpose for this library is to have a set of standard exceptions that can be thrown
 * for the purposes of programmer related problems.
 */

(function (pi)
{
	/**
     * Invalid Operation Exception is used when an operation cannot be performed because the state of
     * of the object performing the operation is invalid to perform that operation, such as a null parameter that should not be.
     */
	var InvalidOperationException = (function (_super)
	{
		__extends(InvalidOperationException, _super);
		/**
         * Create an instance of the Invalid Operation Exception
         * @param functionName
         * @param reason
         * @param innerException
         */
		function InvalidOperationException(functionName, reason, innerException)
		{
			var message = "Invalid Operation in " + functionName + " " + reason;
			_super.call(this, message, innerException);
		}
		return InvalidOperationException;
	}(pi.Exception));
	pi.InvalidOperationException = InvalidOperationException;
	;
})(pi || (pi = {}));
/**
 * The PI Module that contains all Project Insight related functionality.
 * The purpose for this library is to have a set of standard exceptions that can be thrown
 * for the purposes of programmer related problems.
 */
(function (pi)
{
	/**
     * Exception used to indicate that a derrived class functionality has not been implemented.
     */
	var NotImplementedException = (function (_super)
	{
		__extends(NotImplementedException, _super);
		/** Exception constructor. */
		function NotImplementedException(functionName, innerException)
		{
			var message = functionName + " has not been implemented.";
			_super.call(this, message, innerException);
		}
		return NotImplementedException;
	}(pi.Exception));
	pi.NotImplementedException = NotImplementedException;
	;
})(pi || (pi = {}));

// References Determin the build order, try to to create circular loops in your build dependencies.
/**
 *  The PI Module contains everything PI.
 */

(function (pi)
{
	/**
     * Helper Class aids in getting and setting values for various Input Types.  This class is meant to be used primarily with the UI.Get() and UI.Set() Functions.
     */
	var InputElementData = (function ()
	{
		/**
         *  Create an instance of this InputElementData
         */
		function InputElementData()
		{
			this.Elements = [];
		}
		Object.defineProperty(InputElementData.prototype, "Element", {
			/** Get or Set the Element Property (when a single input is returned). */
			get: function ()
			{
				return this.Elements[0];
			},
			set: function (e)
			{
				this.Elements[0] = e;
			},
			enumerable: true,
			configurable: true
		});
		Object.defineProperty(InputElementData.prototype, "IsSingleElement", {
			/** Get Flag indicating that the Element is singular. */
			get: function ()
			{
				return this.Elements.length <= 1;
			},
			enumerable: true,
			configurable: true
		});
		return InputElementData;
	}());
	pi.InputElementData = InputElementData;
	/**
     * Form is a static class that supports setting and retrieving values from an input form.
     */
	var Form = (function ()
	{
		function Form()
		{
		}
		/**
         * Allows you to configure the Form class with a JSON object.
         * @param config
         */
		Form.Init = function (config)
		{
			if (!pi.JSTools.IsUndefinedOrNull(config.GlobalYes))
			{
				Form.GlobalYes = config.GlobalYes;
			}
			if (!pi.JSTools.IsUndefinedOrNull(config.GlobalNo))
			{
				Form.GlobalNo = config.GlobalNo;
			}
			if (!pi.JSTools.IsUndefinedOrNull(config.GlobalOK))
			{
				Form.GlobalOK = config.GlobalOK;
			}
			if (!pi.JSTools.IsUndefinedOrNull(config.GlobalCancel))
			{
				Form.GlobalCancel = config.GlobalCancel;
			}
		};
		/**
         * Gets Data about the form input.
         * @param property - property name to search for.
         * @param scope - identifies the form scope to search within.
         * @return InputElementData - information about the form input elements for this property.
         */
		Form.GetInput = function (property, scope)
		{
			var type = null;
			var $elements = pi.UI.GetJQuery(property, scope);
			var elements = $elements.get();
			if (pi.JSTools.IsUndefined(elements))
			{
				elements = [];
			}
			var returnData = new InputElementData;
			var element = null;
			if (elements.length > 0)
			{
				element = elements[0];
			}
			if (element != null)
			{
				if (element.tagName == "SPAN") {
					$elements = $elements.find("input");
					elements = $elements.get();
					element = elements[0];
				}
			}
			//FormTools.GetFormElementValue(element);
			returnData.Element = element;
			if (element != null)
			{
				// If type is defined assume we are dealing with an normal input control.
				if (!pi.JSTools.IsUndefinedNullOrEmpty(element.type))
				{
					returnData.Type = element.type;
					// if this is a radio button get all radio buttons.
					if (returnData.Type == "radio")
					{
						returnData.Elements = $elements.get();
					}
					return returnData;
				}
				else if (element.tagName == "TABLE" || element.tagName == "DIV")
				{
					if (!pi.JSTools.IsUndefinedNullOrEmpty(element.className))
					{
						if (element.className.indexOf("MetafuseComboBox") >= 0)
						{
							returnData.Type = "MetafuseComboBox";
							var element = pi.UI.GetJQuery(property, scope).get()[0];
							returnData.Element = $("#" + element.id + "_sv").get()[0];
							return returnData;
						}
						else if (element.className.indexOf("PropertySelector") >= 0)
						{
							returnData.Type = "PropertySelector";
							returnData.Elements = pi.UI.GetJQuery(property, scope).get();
							return returnData;
						}
						else if (element.className.indexOf("MetafuseKendoColorPicker") >= 0)
						{
							returnData.Type = "MetafuseKendoColorPicker";
							return returnData;
						}
						else if (element.className.indexOf("MetafuseKendoEditor") >= 0)
						{
							returnData.Type = "MetafuseKendoEditor";
							return returnData;
						}
						else if (element.className.indexOf("radio-button-list") >= 0)
						{
							returnData.Type = "radio-button-list";
							return returnData;
						}
						// Radio List is an ASPX specific thing, so we need to handle it special.
						var radioElement = pi.UI.GetElementById(element.id + "_0");
						if (radioElement != null && radioElement.type != null && radioElement.type == "radio")
						{
							returnData.Type = "radio-button-list";
							returnData.Elements = $elements.find("input").get();
							return returnData;
						}
					}
				}
			}
			return null;
		};
		/**
         * Transforms the input form within a given scope into a JSON object.
         * @param scope - scope of the top element to transform.
         */
		Form.GetObject = function (scope)
		{
			var obj = {};
			var $scopeProperties = pi.UI.GetPropertiesInScope(scope);
			// This jquery chain gets all properties that exist within a subscope and removes them from the current query then selects only elements that are of a value type.
			//STW-2016-10-02--- i'm not sure why we have a not in.   the pi-data-json could have all of the JSON for the object we are updating, if we have properties in the scope, we'd want to get all 
			//of the properties in the scope, regardless of whether or not it is in the JSON or not.   
			$scopeProperties.not("[data-pi-json]").each(function (index, element)
			{
				var property = $(this).attr("data-pi-property");
				obj[property] = Form.Get(property, scope);
			});
			//STW-2016-10-02 we need to think about this parent child relationship more...
			$scopeProperties.filter("[data-pi-parent-scope=" + scope + "]").each(function (index, element)
			{
				var property = $(this).attr("data-pi-property");
				obj[property] = JSON.parse($(this).attr("data-pi-json"));
			});
			return obj;
		};
		/**
         * Get the JSON object attached to a, null if no json is on or associated to the element, this can work for any input that also stores the json on it.
         * @param property - Property to retrieve
         * @param scope - scope to retrieve it from
         */
		Form.GetSelectedJson = function (property, scope)
		{
			var data = Form.GetInput(property, scope);
			if (data.Type == "select-multiple" || data.Type == "select-one")
			{
				var jsonArray = [];
				if (!pi.JSTools.IsUndefinedNullOrEmpty(data.Element.options))
				{
					var selectElement = data.Element;
					var useValue = true;
					if (selectElement.options.length > 1)
					{
						var option1 = selectElement.options[0];
						var option2 = selectElement.options[1];
						if (pi.JSTools.IsUndefinedNullOrEmpty(option1.value) && pi.JSTools.IsUndefinedNullOrEmpty(option2.value))
						{
							useValue = false;
						}
					}
					// Look through the options array to find which options have been marked selected.
					for (var i = 0; i < selectElement.options.length; i++)
					{
						if (selectElement.options[i])
						{
							var json = JSON.parse(pi.UI.GetElementJson(selectElement.options[i]));
							if (json != null)
							{
								jsonArray.push(json);
							}
						}
					}
				}
				return jsonArray;
			}
			else if (data.Type == "MetafuseComboBox")
			{
				// Get the value off of the MetafuseCombobox
				return JSON.parse(pi.UI.GetSelectedJson(data.Element));
			}
			else
			{
				// Get the json off of the 
				return JSON.parse(pi.UI.GetElementJson(data.Element));
			}
			//return null;
		};
		/**
         * Gets the value of the property defined within a given scope.
         * @param property - the property to look for.
         * @param scope - optional value identifies the html elements to search through, defaults to entire body.
         * @param valueSeparator - optional value used to identify the separate values when dealing with multi-select proeprties
         * @return any
         */
		Form.Get = function (property, scope, valueSeparator)
		{
			var $property = pi.UI.GetJQuery(property, scope);
			var id = $property.attr("id");
			var type = "";
			if (pi.JSTools.IsUndefinedNullOrEmpty(id))
			{
				$input = $property.find("input");
				id = $input.attr("id");
				type = $input.attr("type");
			}
			var value = FormTools.Get(id, valueSeparator);

			// convert checkboxes to boolean
			if(type == "checkbox")
			{
				value = value == "on";
			}

			return value;
		};
		/**
         * Set the value of a form element
         * @param value - value to set the element to.
         * @param property - property name to search for.
         * @param scope - identifies the form scope to search within.
         * @param text - (optional) - text that goes with the supplied value, this is utilized only when using selection lists
         * @param valueSeparator - (optional) special separator used to denote values when using a selection list.
         * @param textSeparator - (optional) special separator used to denote display text when using a selection list
         * @param missingOptionArray - (optional) array of missing options to set
         * @param insertOptionMissing - (optional) flag indicating to insert missing options into the selection list
         * @param dropDownSelectWithMatchingValueOrTextOnOptions - flag indicating to select the text whent true
         */
		Form.Set = function (value, property, scope, text, valueSeparator, textSeparator, missingOptionArray, insertOptionMissing, dropDownSelectWithMatchingValueOrTextOnOptions)
		{
			if (pi.JSTools.IsUndefinedNullOrEmpty(value))
			{
				return false;
			}
			if (pi.JSTools.IsUndefinedNullOrEmpty(valueSeparator))
			{
				valueSeparator = ",";
			}
			var elementData = Form.GetInput(property, scope);
			var success = false;
			if (elementData != null)
			{
				// TODO: Selection methods could be greatly improved through the use of hashing over double for loop + empty selection would equate to no loops period in both cases but currently loop through all optons anyway at least once.
				if (elementData.Type == 'select-multiple')
				{
					var valueArray = null;
					var textArray = null;
					// strip out white spaces between values and separators
					value = value.replace(valueSeparator + ' ', valueSeparator);
					valueArray = value.split(valueSeparator);
					if (!pi.JSTools.IsUndefinedOrNull(text))
					{
						text = text.replace(textSeparator + ' ', textSeparator);
						textArray = text.split(textSeparator);
					}
					var selectElement = elementData.Element;
					if (!pi.JSTools.IsUndefinedOrNull(selectElement.options.length))
					{
						// use the value to select which option is selected, otherwise use the text.
						var useValue = true;
						if (selectElement.options.length > 1)
						{
							var option1 = selectElement.options[0];
							var option2 = selectElement.options[1];
							// if the value is not set then we need to use the text to select.
							if (pi.JSTools.IsUndefinedNullOrEmpty(option1.value) && pi.JSTools.IsUndefinedNullOrEmpty(option2.value))
							{
								useValue = false;
							}
						}
						for (var optionIdx = 0; optionIdx < selectElement.options.length; ++optionIdx)
						{
							var option = selectElement.options[optionIdx];
							option.selected = false;
							for (var valueIdx = 0; valueIdx < valueArray.length; ++valueIdx)
							{
								var val = valueArray[valueIdx];
								var foundMatch = false;
								if (useValue)
								{
									if (option.value == val)
									{
										option.selected = true;
										foundMatch = true;
									}
									else if (option.value == "" && val == "")
									{
										option.selected = true;
										foundMatch = true;
									}
									else if (dropDownSelectWithMatchingValueOrTextOnOptions && (option.text == val || jQuery.trim(option.text) == jQuery.trim(val)))
									{
										option.selected = true;
										foundMatch = true;
									}
								}
								else
								{
									if (option.text.toString() == value.toString())
									{
										option.selected = true;
										foundMatch = true;
									}
									else if (option.text == "" && val == "")
									{
										option.selected = true;
										foundMatch = true;
									}
								}
								// if the match was found, then remove the value from the list
								if (foundMatch)
								{
									valueArray.splice(valueIdx, 1);
									if (textArray)
									{
										textArray.splice(valueIdx, 1);
									}
									break;
								}
							}
						}
					}
					// if the value array is not emptied then we need to insert options.
					if (valueArray.length > 0)
					{
						if (missingOptionArray != null || insertOptionMissing)
						{
							if (missingOptionArray == null)
							{
								missingOptionArray = new Array();
							}
							for (var i = 0; i < valueArray.length; ++i)
							{
								var val = valueArray[i];
								var txt = val;
								if (!pi.JSTools.IsUndefinedOrNull(textArray) && textArray.length > i)
								{
									txt = textArray[i];
								}
								var opt = { text: txt, value: val };
								missingOptionArray[missingOptionArray.length] = opt;
							}
						}
						if (insertOptionMissing && missingOptionArray.length > 0)
						{
							for (var i = missingOptionArray.length - 1; i >= 0; --i)
							{
								var missingOption = missingOptionArray[i];
								Form.InsertListValueAndText(elementData, missingOption.value, missingOption.text, true, 0);
							}
							success = true;
						}
					}
					else
					{
						return true;
					}
					if (pi.jQueryMobileInputRefreshEnabled)
					{
						$(elementData.Element).selectmenu('refresh');
					}
				}
				else if (elementData.Type == 'select-one')
				{
					var selectElement = elementData.Element;
					if (!pi.JSTools.IsUndefinedOrNull(selectElement.options.length))
					{
						var useValue = true;
						if (selectElement.options.length > 1)
						{
							var option1 = selectElement.options[0];
							var option2 = selectElement.options[1];
							// if the value is not set then we need to use the text to select.
							if (pi.JSTools.IsUndefinedNullOrEmpty(option1.value) && pi.JSTools.IsUndefinedNullOrEmpty(option2.value))
							{
								useValue = false;
							}
						}
						for (var optionIdx = 0; optionIdx < selectElement.options.length; ++optionIdx)
						{
							var option = selectElement.options[optionIdx];
							if (useValue)
							{
								if (option.value.toString() == value.toString())
								{
									option.selected = true;
									success = true;
								}
								else if (option.value == "" && value == "")
								{
									option.selected = true;
									success = true;
								}
								else if (dropDownSelectWithMatchingValueOrTextOnOptions && (option.text == value || jQuery.trim(option.text) == jQuery.trim(value)))
								{
									option.selected = true;
									success = true;
								}
							}
							else
							{
								if (option.text.toString() == value.toString())
								{
									option.selected = true;
									success = true;
								}
								else if (option.text == "" && value == "")
								{
									option.selected = true;
									success = true;
								}
							}
							if (success)
							{
								break;
							}
						}
					}
					if (!success)
					{
						if (pi.JSTools.IsUndefinedOrNull(missingOptionArray))
						{
							var opt = { text: text, value: value };
							missingOptionArray[missingOptionArray.length] = opt;
						}
						if (insertOptionMissing)
						{
							Form.InsertListValueAndText(elementData, value, text, true, 0);
							success = true;
						}
					}
					if (pi.jQueryMobileInputRefreshEnabled)
					{
						$(elementData.Element).selectmenu('refresh');
					}
				}
				else if (elementData.Type == 'checkbox')
				{
					value = value.toString().toLowerCase();
					var inputElement = elementData.Element;
					if (value == "on" || value == "true" || value == "1" || value == "checked" || value == "yes" || value == "active" || value == this.GlobalYes.toLowerCase())
					{
						inputElement.checked = true;
						success = true;
					}
					else
					{
						inputElement.checked = false;
						success = true;
					}
					// Rendering using jquery mobile
					if (pi.jQueryMobileInputRefreshEnabled)
					{
						$(elementData.Element).checkboxradio('refresh');
					}
				}
				else if (elementData.Type == 'radio')
				{
					for (var i = 0; i < elementData.Elements.length; ++i)
					{
						var radioElement = elementData.Elements[i];
						if (radioElement.value == value)
						{
							radioElement.checked = true;
							success = true;
							break;
						}
					}
				}
				else if (elementData.Type == "MetafuseComboBox")
				{
					var metafuseComboBoxElement = window[elementData.Element.id + "_obj"];
					if (!pi.JSTools.IsUndefinedOrNull(metafuseComboBoxElement))
					{
						return metafuseComboBoxElement.SetFormElementValue(value, text, valueSeparator, missingOptionArray, insertOptionMissing);
					}
				}
				else if (elementData.Type == "MetafuseKendoColorPicker")
				{
					console.error("not implemented");
				}
				else if (elementData.Type == "MetafuseKendoEditor")
				{
					console.error("not implemented");
				}
				else if (elementData.Type == "PropertySelector")
				{
					console.error("not implemented");
				}
				else if (elementData.Type == "radio-button-list")
				{
					console.error("not implemented");
				}
				else
				{
					var inputElement = elementData.Element;
					inputElement.value = value;
					success = true;
				}
			}
			return success;
		};
		/**
         * Insert a Value and Text for a Selection List or combo box
         * @param elementData - InputElementData about the list
         * @param value - value to insert
         * @param text - text to insert
         * @param defaultSelected - boolean indicator that it should be selected.
         * @param index - Location within the options array to insert it into.
         */
		Form.InsertListValueAndText = function (elementData, value, text, defaultSelected, index)
		{
			if (!pi.JSTools.IsUndefinedOrNull(elementData))
			{
				if (elementData.Type == 'select-one' || elementData.Type == 'select-multiple')
				{
					var newOption = new Option();
					newOption.text = text;A
					newOption.value = value;
					if (defaultSelected)
					{
						newOption.selected = true;
					}
					var selectElement = elementData.Element;
					if (selectElement.length > 0)
					{
						if (!pi.JSTools.IsUndefinedOrNull(index))
						{
							index = 0;
						}
						var existingOption = selectElement.options[index];
						try
						{
							selectElement.add(newOption, existingOption);
						}
						catch (ex)
						{
							selectElement.add(newOption, index);
						}
					}
					else
					{
						// add one to the end.
						selectElement.options[0] = newOption;
					}
				}
				else if (elementData.Type == "MetafuseComboBox")
				{
					var metafuseComboBoxElement = window[elementData.Element.id + "_obj"];
					if (!pi.JSTools.IsUndefinedOrNull(metafuseComboBoxElement))
					{
						metafuseComboBoxElement.AddAvailableRow(value, text, defaultSelected);
					}
				}
				else
				{
					// this should probably be an exception...
					console.error("insert form field and text not implemented for element type: " + elementData.Type + " id: " + elementData.Element.id + " value: " + value + " text: " + text);
				}
			}
		};
		/** String that defines what the global YES string is. */
		Form.GlobalYes = "Yes";
		/** String that defines what the global No string is */
		Form.GlobalNo = "No";
		/** String that defines what the global Okay string is */
		Form.GlobalOK = "OK";
		/** String that defines what the global Cancel string is. */
		Form.GlobalCancel = "Cancel";
		return Form;
	}());
	pi.Form = Form;
})(pi || (pi = {}));

(function (pi)
{
	/**
     * Formally defined hash table object for working with Javascript Hashed Objects.
     * @warning objects cannot be used as keys because javascript views all "objects" as the same property.  Use something that JS views as unique such as numbers and strings.
     */
	var HashTable = (function ()
	{
		/**
         * Create an instance of the HashTable object.
         */
		function HashTable()
		{
			this.items = {};
		}
		/**
         * Add a new value to the hash table.
         * @param key - key to use, can be any object but cannot be undefined, null, or empty string
         * @param value - value to associate with that key.
         * @return boolean - true if the item was successfully inserted into the hash table.
         */
		HashTable.prototype.Add = function (key, value)
		{
			if (!pi.JSTools.IsUndefinedNullOrEmpty(key))
			{
				this.items[key] = value;
				return true;
			}
			return false;
		};
		/**
         * Remove an item from the hash table.
         * @param key - key of the item to remove.
         * @return object - the removed object from this hash table.
         */
		HashTable.prototype.Remove = function (key)
		{
			var removed = null;
			if (this.HasItem(key))
			{
				removed = this.items[key];
				delete this.items[key];
			}
			return removed;
		};
		/**
         * Determine if the item exists within the hash
         * @param key - key of the item to check.
         * @return boolean - value indicating the item exists when true.
         */
		HashTable.prototype.HasItem = function (key)
		{
			if (!pi.JSTools.IsUndefinedNullOrEmpty(key))
			{
				return this.items.hasOwnProperty(key);
			}
			return false;
		};
		Object.defineProperty(HashTable.prototype, "Keys", {
			/** Get an array of key objects */
			get: function ()
			{
				var keys = [];
				for (var key in this.items)
				{
					if (this.HasItem(key))
					{
						keys.push(key);
					}
				}
				return keys;
			},
			enumerable: true,
			configurable: true
		});
		Object.defineProperty(HashTable.prototype, "Values", {
			/** Get an array of Values contained within this hash table. */
			get: function ()
			{
				var values = [];
				for (var key in this.items)
				{
					if (this.HasItem(key))
					{
						values.push(this.items[key]);
					}
				}
				return values;
			},
			enumerable: true,
			configurable: true
		});
		/**
         * Gets the item associated with the specified key
         * @param key - key you want the item associated to.
         * @return object - the object associated with this key, if one is not found then null is returned.
         */
		HashTable.prototype.Get = function (key)
		{
			if (this.HasItem(key))
			{
				return this.items[key];
			}
			return null;
		};
		/**
         * Used to easily iterate through each key value pair within this hash table
         * @param fn - function or lamda function to call when iterating through each element.
         */
		HashTable.prototype.each = function (fn)
		{
			for (var k in this.items)
			{
				if (this.HasItem(k))
				{
					fn(k, this.items[k]);
				}
			}
		};
		/**
         * Remove all values and keys from the hash table.
         */
		HashTable.prototype.Clear = function ()
		{
			this.items = {};
		};
		return HashTable;
	}());
	pi.HashTable = HashTable;
})(pi || (pi = {}));
//MFICOMPRESS
/**
 * The PI Module that contains all Project Insight related functionality.
 */

(function (pi)
{
	/**
    * Root of Segment of the PI URL
    */
	pi.Root = "";
	/**
     * Return URL of the client's current session.
     */
	pi.Rtn = "";
	/**
     * The Default scope to perform searches or queries.
     */
	pi.DefaultScope = "body";
	/**
     *  Gobal value to enable rendering for jquery mobile controls used in Set Form Element Value.
     */
	pi.jQueryMobileInputRefreshEnabled = false;
	/**
     * boolean value that allows the override of specific behavior when in this mode.
     */
	pi.IsDevelopmentMode = false;
	/**
     * Called to initialize the PI Module with important values.
     * @param config - config object to pass in and initialize the pi module settings.
     */
	function Init(config)
	{
		if (!JSTools.IsUndefinedOrNull(config.Root))
		{
			pi.Root = config.Root;
		}
		if (!JSTools.IsUndefinedOrNull(config.Rtn))
		{
			pi.Rtn = config.Rtn;
		}
		if (!JSTools.IsUndefinedOrNull(config.DefaultScope))
		{
			pi.DefaultScope = config.DefaultScope;
		}
		if (!JSTools.IsUndefinedOrNull(config.jQueryMobileInputRefreshEnabled))
		{
			pi.jQueryMobileInputRefreshEnabled = config.jQueryMobileInputRefreshEnabled;
		}
		if (!JSTools.IsUndefinedOrNull(config.Version))
		{
			pi.Version = config.Version;
		}
		if (!JSTools.IsUndefinedOrNull(config.TypeSDK))
		{
			pi.TypeSDK = config.TypeSDK;
		}
		if (!JSTools.IsUndefinedOrNull(config.TypeModel))
		{
			pi.TypeModel = config.TypeModel;
		}
	}
	pi.Init = Init;
	/**
     *  JSTools simply contains useful functions that help with JavaScript objects.
     *  This could have been built as a class with Static Functions as well, but since no instantiation of any object variables are required a module with functions works as well.
     */
	var JSTools;
	(function (JSTools)
	{
		/**
         * Determines if the supplied object is an array or not.
         * @param object - object to test.
         * @return boolean - indicating this object is an array
         */
		function IsArray(object)
		{
			return JSTools.IsObject(object) && object.constructor == Array;
		}
		JSTools.IsArray = IsArray;
		/**
         * Determines if the supplied object is a boolean or not
         * @param object - object to test.
         * @return boolean - indicating this object is a boolean
         */
		function IsBoolean(object)
		{
			return typeof object === 'boolean';
		}
		JSTools.IsBoolean = IsBoolean;
		/**
         * Determines if the supplied object is a function or now.
         * @param object - object to test.
         * @return boolean - indicating this object is a function
         */
		function IsFunction(object)
		{
			return typeof object === 'function';
		}
		JSTools.IsFunction = IsFunction;
		/**
         * Detects if the provided object is a string type
         * @param object - object to test.
         * @return boolean - indicating this object is a string
         */
		function IsString(object)
		{
			return typeof object === 'string';
		}
		JSTools.IsString = IsString;
		/**
         * Determine if the supplied object is a number
         * @param object - object to test.
         * @return boolean - indicating this object is a number
         */
		function IsNumber(object)
		{
			return ((typeof object === 'number' || !isNaN(parseFloat(object))) && isFinite(object));
		}
		JSTools.IsNumber = IsNumber;
		/**
         * Determine if the supplied object is an actual object
         * @param object - object to test.
         * @return boolean - indicating this object is an object
         */
		function IsObject(object)
		{
			return object != null && typeof object === 'object';
		}
		JSTools.IsObject = IsObject;
		/**
         * Determine if the object is null
         * @param object - object to test.
         * @return boolean - indicating this object is actually null
         */
		function IsNull(object)
		{
			return object == null;
		}
		JSTools.IsNull = IsNull;
		/**
         * Determines if the object is undefined.
         * @param object - object to test.
         * @return boolean - flag indicating this object is a undefined
         */
		function IsUndefined(object)
		{
			return object == undefined;
		}
		JSTools.IsUndefined = IsUndefined;
		/**
         * Determines if the supplied object is undefined or null.
         * @param object - object to test.
         * @return boolean - indicating this object is a undefined or null when true.
         */
		function IsUndefinedOrNull(object)
		{
			return JSTools.IsUndefined(object) || JSTools.IsNull(object);
		}
		JSTools.IsUndefinedOrNull = IsUndefinedOrNull;
		/**
         * Determines if the supplied object is undefined, null, or an empty string.
         * @param object - object to test.
         * @return - boolean indicating this object is a undefined, null, or an empty string when true.
         */
		function IsUndefinedNullOrEmpty(object)
		{
			return JSTools.IsUndefinedOrNull(object) || object == "";
		}
		JSTools.IsUndefinedNullOrEmpty = IsUndefinedNullOrEmpty;
	})(JSTools = pi.JSTools || (pi.JSTools = {}));
})(pi || (pi = {}));
// All the config values are optional so you can omit them where you need.
// pi.Init({ Rtn: "/tasks/99292910929394920203", Root:"/" }); 
/**
 * The PI Module that contains all Project Insight related functionality.
 */

(function (pi)
{
	var DateTools;
	(function (DateTools)
	{
		/**
        * Defines the Time of Day as AM or PM.
        */
		var ETimeType = (function ()
		{
			function ETimeType()
			{
			}
			/**
             * finds the enum based on the value.
             * @param value - numeric value to find.
             */
			ETimeType.FindByValue = function (value)
			{
				switch (value)
				{
					case 0:
						return ETimeType.AM;
					case 1:
						return ETimeType.PM;
				}
			};
			ETimeType.AM = { Name: "AM", Value: 0 };
			ETimeType.PM = { Name: "PM", Value: 1 };
			return ETimeType;
		}());
		DateTools.ETimeType = ETimeType;
		/**
         * Defines the days of the Week.
         */
		var EDayType = (function ()
		{
			function EDayType()
			{
			}
			/**
             * finds the enum based on the value.
             * @param value - numeric value to find.
             */
			EDayType.FindByValue = function (value)
			{
				switch (value)
				{
					case 0:
						return EDayType.Sunday;
					case 1:
						return EDayType.Monday;
					case 2:
						return EDayType.Tuesday;
					case 3:
						return EDayType.Wednesday;
					case 4:
						return EDayType.Thursday;
					case 5:
						return EDayType.Friday;
					case 6:
						return EDayType.Saturday;
				}
			};
			EDayType.Sunday = { Name: "Sunday", Value: 0 };
			EDayType.Monday = { Name: "Monday", Value: 1 };
			EDayType.Tuesday = { Name: "Tuesday", Value: 2 };
			EDayType.Wednesday = { Name: "Wednesday", Value: 3 };
			EDayType.Thursday = { Name: "Thursday", Value: 4 };
			EDayType.Friday = { Name: "Friday", Value: 5 };
			EDayType.Saturday = { Name: "Saturday", Value: 6 };
			return EDayType;
		}());
		DateTools.EDayType = EDayType;
		/**
         * Defines the months whose values reflect javascript values for months.
         */
		var EMonthType = (function ()
		{
			function EMonthType()
			{
			}
			/**
             * finds the enum based on the value.
             * @param value - numeric value to find.
             */
			EMonthType.FindByValue = function (value)
			{
				switch (value)
				{
					case 0:
						return EMonthType.January;
					case 1:
						return EMonthType.February;
					case 2:
						return EMonthType.March;
					case 3:
						return EMonthType.April;
					case 4:
						return EMonthType.May;
					case 5:
						return EMonthType.June;
					case 6:
						return EMonthType.July;
					case 7:
						return EMonthType.August;
					case 8:
						return EMonthType.September;
					case 9:
						return EMonthType.October;
					case 10:
						return EMonthType.November;
					case 11:
						return EMonthType.December;
				}
			};
			EMonthType.January = { Name: "January", Value: 0 };
			EMonthType.February = { Name: "February", Value: 1 };
			EMonthType.March = { Name: "March", Value: 2 };
			EMonthType.April = { Name: "April", Value: 3 };
			EMonthType.May = { Name: "May", Value: 4 };
			EMonthType.June = { Name: "June", Value: 5 };
			EMonthType.July = { Name: "July", Value: 6 };
			EMonthType.August = { Name: "August", Value: 7 };
			EMonthType.September = { Name: "September", Value: 8 };
			EMonthType.October = { Name: "October", Value: 9 };
			EMonthType.November = { Name: "November", Value: 10 };
			EMonthType.December = { Name: "December", Value: 11 };
			return EMonthType;
		}());
		DateTools.EMonthType = EMonthType;
		/**
         * Performs Localizations for Dates based on the User's current date configuration.
         */
		var DateLocalization = (function ()
		{
			function DateLocalization(monthConfig, dayConfig, timeConfig, localizedDateFormatConfig)
			{
				this.Timezone = 0;
				this.HasDaylightSavings = false;
				// Setup the Defaults for the Localization System.
				this.Months = {
					January: { DateType: EMonthType.January, FullName: "Januaray", ShortName: "Jan", Initial: "J" },
					February: { DateType: EMonthType.February, FullName: "February", ShortName: "Feb", Initial: "F" },
					March: { DateType: EMonthType.March, FullName: "March", ShortName: "Mar", Initial: "M" },
					April: { DateType: EMonthType.April, FullName: "April", ShortName: "Apr", Initial: "A" },
					May: { DateType: EMonthType.May, FullName: "May", ShortName: "May", Initial: "M" },
					June: { DateType: EMonthType.June, FullName: "June", ShortName: "Jun", Initial: "J" },
					July: { DateType: EMonthType.July, FullName: "July", ShortName: "Jul", Initial: "J" },
					August: { DateType: EMonthType.August, FullName: "August", ShortName: "Aug", Initial: "A" },
					September: { DateType: EMonthType.September, FullName: "September", ShortName: "Sep", Initial: "S" },
					October: { DateType: EMonthType.October, FullName: "October", ShortName: "Oct", Initial: "O" },
					November: { DateType: EMonthType.November, FullName: "November", ShortName: "Nov", Initial: "N" },
					December: { DateType: EMonthType.December, FullName: "December", ShortName: "Dec", Initial: "D" }
				};
				this.Days = {
					Sunday: { DateType: EDayType.Sunday, FullName: "Sunday", ShortName: "Sun", Initial: "S" },
					Monday: { DateType: EDayType.Monday, FullName: "Monday", ShortName: "Mon", Initial: "M" },
					Tuesday: { DateType: EDayType.Tuesday, FullName: "Tuesday", ShortName: "Tue", Initial: "T" },
					Wednesday: { DateType: EDayType.Wednesday, FullName: "Wednesday", ShortName: "Wed", Initial: "W" },
					Thursday: { DateType: EDayType.Thursday, FullName: "Thursday", ShortName: "Thr", Initial: "T" },
					Friday: { DateType: EDayType.Friday, FullName: "Friday", ShortName: "Fri", Initial: "F" },
					Saturday: { DateType: EDayType.Saturday, FullName: "Saturday", ShortName: "Sat", Initial: "S" },
				};
				this.DateFormats = {
					DateTime: "MM/dd/yyyy h:mm",
					LongDateOnly: "dddd, MMMM dd, yyyy",
					DateOnly: "MM/dd/yyyy",
					SchDateTime: "MMM MM/dd/yy h:mm",
					SchDateTimeTopHour: "ddd MM/dd/yy h",
					SchShortDateOnly: "MM/dd/yy",
					USSchShortDateOnly: "MM/dd/yyyy",
					JSShortDateOnly: "yyyy/M/d",
					GanttWeekDate: "MMM dd, yyyy",
					GanttMonthDate: "MMM 'yy",
					GanttShortDate: "M/d/yyyy"
				};
				this.Time = {
					AM: { DateType: ETimeType.AM, FullName: "AM", ShortName: "am", Initial: "a" },
					PM: { DateType: ETimeType.PM, FullName: "PM", ShortName: "pm", Initial: "p" }
				};
				this.SetByConfig(monthConfig, dayConfig, timeConfig, localizedDateFormatConfig);
			}
			/**
             * Set up the localization utility by configuration.
             * @param monthConfig - month configuration values
             * @param dayConfig - day configuraiton values
             * @param timeConfig - time configuration values
             * @param localizedDateFormatConfig - standarized format strings configuraiton values.
             */
			DateLocalization.prototype.SetByConfig = function (monthConfig, dayConfig, timeConfig, localizedDateFormatConfig)
			{
				if (!pi.JSTools.IsUndefinedOrNull(monthConfig))
				{
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.January))
					{
						this.Months.January = monthConfig.January;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.January.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.January.DateType))
						{
							this.Months.January.DateType = EMonthType.FindByValue(monthConfig.January.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.February))
					{
						this.Months.February = monthConfig.February;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.February.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.February.DateType))
						{
							this.Months.February.DateType = EMonthType.FindByValue(monthConfig.February.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.March))
					{
						this.Months.March = monthConfig.March;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.March.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.March.DateType))
						{
							this.Months.March.DateType = EMonthType.FindByValue(monthConfig.March.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.April))
					{
						this.Months.April = monthConfig.April;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.April.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.April.DateType))
						{
							this.Months.April.DateType = EMonthType.FindByValue(monthConfig.April.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.May))
					{
						this.Months.May = monthConfig.May;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.May.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.May.DateType))
						{
							this.Months.May.DateType = EMonthType.FindByValue(monthConfig.May.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.June))
					{
						this.Months.June = monthConfig.June;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.June.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.June.DateType))
						{
							this.Months.June.DateType = EMonthType.FindByValue(monthConfig.June.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.July))
					{
						this.Months.July = monthConfig.July;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.July.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.July.DateType))
						{
							this.Months.July.DateType = EMonthType.FindByValue(monthConfig.July.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.August))
					{
						this.Months.August = monthConfig.August;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.August.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.August.DateType))
						{
							this.Months.August.DateType = EMonthType.FindByValue(monthConfig.August.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.September))
					{
						this.Months.September = monthConfig.September;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.September.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.September.DateType))
						{
							this.Months.September.DateType = EMonthType.FindByValue(monthConfig.September.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.October))
					{
						this.Months.October = monthConfig.October;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.October.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.October.DateType))
						{
							this.Months.October.DateType = EMonthType.FindByValue(monthConfig.October.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.November))
					{
						this.Months.November = monthConfig.November;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.November.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.November.DateType))
						{
							this.Months.November.DateType = EMonthType.FindByValue(monthConfig.November.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(monthConfig.December))
					{
						this.Months.December = monthConfig.December;
						if (!pi.JSTools.IsUndefinedOrNull(monthConfig.December.DateId) && pi.JSTools.IsUndefinedOrNull(monthConfig.December.DateType))
						{
							this.Months.December.DateType = EMonthType.FindByValue(monthConfig.December.DateId);
						}
					}
				}
				if (!pi.JSTools.IsUndefinedOrNull(dayConfig))
				{
					if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Sunday))
					{
						this.Days.Sunday = dayConfig.Sunday;
						if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Sunday.DateId) && pi.JSTools.IsUndefinedOrNull(dayConfig.Sunday.DateType))
						{
							this.Days.Sunday.DateType = EDayType.FindByValue(dayConfig.Sunday.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Monday))
					{
						this.Days.Monday = dayConfig.Monday;
						if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Monday.DateId) && pi.JSTools.IsUndefinedOrNull(dayConfig.Monday.DateType))
						{
							this.Days.Monday.DateType = EDayType.FindByValue(dayConfig.Monday.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Tuesday))
					{
						this.Days.Tuesday = dayConfig.Tuesday;
						if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Tuesday.DateId) && pi.JSTools.IsUndefinedOrNull(dayConfig.Tuesday.DateType))
						{
							this.Days.Tuesday.DateType = EDayType.FindByValue(dayConfig.Tuesday.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Wednesday))
					{
						this.Days.Wednesday = dayConfig.Wednesday;
						if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Wednesday.DateId) && pi.JSTools.IsUndefinedOrNull(dayConfig.Wednesday.DateType))
						{
							this.Days.Wednesday.DateType = EDayType.FindByValue(dayConfig.Wednesday.DateId);
						}
					}
					if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Thursday))
					{
						this.Days.Thursday = dayConfig.Thursday;
						this.Days.Thursday.DateType = EDayType.Thursday;
					}
					if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Friday))
					{
						this.Days.Friday = dayConfig.Friday;
						this.Days.Friday.DateType = EDayType.Friday;
					}
					if (!pi.JSTools.IsUndefinedOrNull(dayConfig.Saturday))
					{
						this.Days.Saturday = dayConfig.Saturday;
						this.Days.Saturday.DateType = EDayType.Saturday;
					}
				}
				if (!pi.JSTools.IsUndefinedOrNull(timeConfig))
				{
					if (!pi.JSTools.IsUndefinedOrNull(timeConfig.AM))
					{
						this.Time.AM = timeConfig.AM;
						this.Time.AM.DateType = ETimeType.AM;
					}
					if (!pi.JSTools.IsUndefinedOrNull(timeConfig.PM))
					{
						this.Time.PM = timeConfig.PM;
						this.Time.PM.DateType = ETimeType.PM;
					}
				}
				if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig))
				{
					if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig.DateTime))
					{
						this.DateFormats.DateTime = localizedDateFormatConfig.DateTime;
					}
					if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig.LongDateOnly))
					{
						this.DateFormats.LongDateOnly = localizedDateFormatConfig.LongDateOnly;
					}
					if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig.DateOnly))
					{
						this.DateFormats.DateOnly = localizedDateFormatConfig.DateOnly;
					}
					if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig.SchDateTime))
					{
						this.DateFormats.SchDateTime = localizedDateFormatConfig.SchDateTime;
					}
					if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig.SchDateTimeTopHour))
					{
						this.DateFormats.SchDateTimeTopHour = localizedDateFormatConfig.SchDateTimeTopHour;
					}
					if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig.SchShortDateOnly))
					{
						this.DateFormats.SchShortDateOnly = localizedDateFormatConfig.SchShortDateOnly;
					}
					if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig.JSShortDateOnly))
					{
						this.DateFormats.JSShortDateOnly = localizedDateFormatConfig.JSShortDateOnly;
					}
					if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig.GanttWeekDate))
					{
						this.DateFormats.GanttWeekDate = localizedDateFormatConfig.GanttWeekDate;
					}
					if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig.GanttMonthDate))
					{
						this.DateFormats.GanttMonthDate = localizedDateFormatConfig.GanttMonthDate;
					}
					if (!pi.JSTools.IsUndefinedOrNull(localizedDateFormatConfig.GanttShortDate))
					{
						this.DateFormats.GanttShortDate = localizedDateFormatConfig.GanttShortDate;
					}
				}
			};
			/**
            * Creates a Localized Date Format String
            * @localMonth - The Localized Month Information
            * @localDay - The localized Day Information
            * @date - the date to localize
            * @dateFormat - the format of the date to present.
            */
			DateLocalization.prototype.FormatDate = function (localMonth, localDay, localTime, date, dateFormat)
			{
				var dateString = dateFormat;
				var index = -1;
				var repStr = "";
				var regEx = /./g;
				index = dateFormat.search("yyyy");
				if (index != -1)
				{
					repStr = date.getFullYear().toString();
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 4);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("yyyy", repStr);
				}
				index = dateFormat.search("yy");
				if (index != -1)
				{
					var position = date.getFullYear().toString().length - 2;
					repStr = date.getFullYear().toString().slice(position, date.getFullYear().toString().length);
					dateString = dateString.substr(0, index) + repStr + dateString.substring(index + 2);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("yy", repStr);
				}
				index = dateFormat.search("MMMM");
				if (index != -1)
				{
					repStr = localMonth.FullName;
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 4);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("MMMM", repStr);
				}
				index = dateFormat.search("MMM");
				if (index != -1)
				{
					repStr = localMonth.ShortName;
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 3);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("MMM", repStr);
				}
				index = dateFormat.search("MM");
				if (index != -1)
				{
					repStr = String("0" + (localMonth.DateType.Value + 1)).slice(-2);
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 2);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("MM", repStr);
				}
				index = dateFormat.search("M");
				if (index != -1)
				{
					repStr = (localMonth.DateType.Value + 1).toString();
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 1);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("M", repStr);
				}
				index = dateFormat.search("dddd");
				if (index != -1)
				{
					repStr = localDay.FullName;
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 4);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("dddd", repStr);
				}
				index = dateFormat.search("ddd");
				if (index != -1)
				{
					repStr = localDay.ShortName;
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 3);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("ddd", repStr);
				}
				index = dateFormat.search("dd");
				if (index != -1)
				{
					repStr = String("0" + date.getDate().toString()).slice(-2);
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 2);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("dd", repStr);
				}
				index = dateFormat.search("d");
				if (index != -1)
				{
					repStr = date.getDate().toString();
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 1);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("d", repStr);
				}
				index = dateFormat.search("hh");
				if (index != -1)
				{
					repStr = String("0" + (date.getHours() % 12).toString()).slice(-2);
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 2);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("hh", repStr);
				}
				index = dateFormat.search("h");
				if (index != -1)
				{
					repStr = (date.getHours() % 12).toString();
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 1);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("h", repStr);
				}
				index = dateFormat.search("HH");
				if (index != -1)
				{
					repStr = String("0" + date.getHours()).slice(-2);
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 2);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("HH", repStr);
				}
				index = dateFormat.search("H");
				if (index != -1)
				{
					repStr = date.getHours().toString();
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 1);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("H", repStr);
				}
				index = dateFormat.search("mm");
				if (index != -1)
				{
					repStr = String("0" + date.getMinutes()).slice(-2);
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 2);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("mm", repStr);
				}
				index = dateFormat.search("m");
				if (index != -1)
				{
					repStr = date.getMinutes().toString();
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 1);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("m", repStr);
				}
				index = dateFormat.search("ss");
				if (index != -1)
				{
					repStr = String("0" + date.getMinutes()).slice(-2);
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 2);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("ss", repStr);
				}
				index = dateFormat.search("s");
				if (index != -1)
				{
					//dateString = dateString.replace("s", date.getMinutes().toString());
					dateString = dateString.substr(0, index) + date.getMinutes().toString() + dateString.substr(index + 1);
					dateFormat = dateFormat.replace("s", " ");
					repStr = date.getMinutes().toString();
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 1);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("s", repStr);
				}
				index = dateFormat.search("tt");
				if (index != -1)
				{
					repStr = localTime.FullName;
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 2);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("tt", repStr);
				}
				index = dateFormat.search("t");
				if (index != -1)
				{
					repStr = localTime.Initial;
					dateString = dateString.substr(0, index) + repStr + dateString.substr(index + 1);
					repStr = repStr.replace(regEx, " ");
					dateFormat = dateFormat.replace("t", repStr);
				}
				return dateString;
			};
			/**
            * Converts a Date to a localized date time format.
            * @param	date - date to convert
            */
			DateLocalization.prototype.GetDateTimeString = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.DateTime);
				return dateString;
			};
			/**
            * Converts a Date to a localized long date only format.
            * @param	date - date to convert
            */
			DateLocalization.prototype.GetLongDateOnly = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.LongDateOnly);
				return dateString;
			};
			/**
            * Converts a Date to a localized date only format.
            * @param	date - date to convert
            */
			DateLocalization.prototype.GetDateOnly = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.DateOnly);
				return dateString;
			};
			/**
            * Converts a Date to a localized date scheduled date time format.
            * @param	date - date to convert
            */
			DateLocalization.prototype.GetSchDateTime = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.SchDateTime);
				return dateString;
			};
			/**
            * Converts a Date to a localized scheduling date with top hour format.
            * @param	date - date to convert
            */
			DateLocalization.prototype.GetSchDateTimeTopHour = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.SchDateTimeTopHour);
				return dateString;
			};
			/**
            * Converts a Date to a localized short version of the Date only format.
            * @param	date - date to convert
            */
			DateLocalization.prototype.GetSchShortDateOnly = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.SchShortDateOnly);
				return dateString;
			};
			/**
            * Converts a date to the US Scheduled short version of the Date only format
            * @param	date - date to convert.
            */
			DateLocalization.prototype.GetUSSchShortDateOnly = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.USSchShortDateOnly);
				return dateString;
			};
			/**
            * Converts a Date to a JavaScript Friendly Short Date
            * @param	date - date to convert
            */
			DateLocalization.prototype.GetJSShortDateOnly = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.JSShortDateOnly);
				return dateString;
			};
			/**
            * Converts a Date to a localized date string for the Gantt Week.
            * @param	date - date to convert
            */
			DateLocalization.prototype.GetGanttWeekDate = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.GanttWeekDate);
				return dateString;
			};
			/**
            * Converts a Date to a localized date string for the Gantt Week.
            * @param	date - date to convert
            */
			DateLocalization.prototype.GetGanttMonthDate = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.GanttMonthDate);
				return dateString;
			};
			DateLocalization.prototype.GetGanttShortDate = function (date)
			{
				var dateString = "";
				var localMonth = this.GetMonthType(date.getMonth());
				var localDay = this.GetDayType(date.getDay());
				var localTime = this.GetTimeType(date.getHours());
				dateString = this.FormatDate(localMonth, localDay, localTime, date, this.DateFormats.GanttShortDate);
				return dateString;
			};
			/**
            * Gets the LocalizedDateType of the supplied month number
            * @param	month - numerical value of the month.
            * @return	LocalizedDateType
            */
			DateLocalization.prototype.GetMonthType = function (month)
			{
				if (month == this.Months.January.DateType.Value)
				{
					return this.Months.January;
				}
				else if (month == this.Months.February.DateType.Value)
				{
					return this.Months.February;
				}
				else if (month == this.Months.March.DateType.Value)
				{
					return this.Months.March;
				}
				else if (month == this.Months.April.DateType.Value)
				{
					return this.Months.April;
				}
				else if (month == this.Months.May.DateType.Value)
				{
					return this.Months.May;
				}
				else if (month == this.Months.June.DateType.Value)
				{
					return this.Months.June;
				}
				else if (month == this.Months.July.DateType.Value)
				{
					return this.Months.July;
				}
				else if (month == this.Months.August.DateType.Value)
				{
					return this.Months.August;
				}
				else if (month == this.Months.September.DateType.Value)
				{
					return this.Months.September;
				}
				else if (month == this.Months.October.DateType.Value)
				{
					return this.Months.October;
				}
				else if (month == this.Months.November.DateType.Value)
				{
					return this.Months.November;
				}
				else if (month == this.Months.December.DateType.Value)
				{
					return this.Months.December;
				}
			};
			/**
            * Gets the LocalizedDateType of the supplied day number
            * @param	day - numerical value of the day.
            * @return	LocalizedDateType
            */
			DateLocalization.prototype.GetDayType = function (day)
			{
				if (day == this.Days.Sunday.DateType.Value)
				{
					return this.Days.Sunday;
				}
				else if (day == this.Days.Monday.DateType.Value)
				{
					return this.Days.Monday;
				}
				else if (day == this.Days.Tuesday.DateType.Value)
				{
					return this.Days.Tuesday;
				}
				else if (day == this.Days.Wednesday.DateType.Value)
				{
					return this.Days.Wednesday;
				}
				else if (day == this.Days.Thursday.DateType.Value)
				{
					return this.Days.Thursday;
				}
				else if (day == this.Days.Friday.DateType.Value)
				{
					return this.Days.Friday;
				}
				else if (day == this.Days.Saturday.DateType.Value)
				{
					return this.Days.Saturday;
				}
			};
			/**
            * Gets the LocalizedDateType of the supplied hour number
            * @param	hour - numerical value of the hour.
            * @return	LocalizedDateType
            */
			DateLocalization.prototype.GetTimeType = function (hour)
			{
				if (hour >= 12)
				{
					return this.Time.PM;
				}
				else
				{
					return this.Time.AM;
				}
			};
			return DateLocalization;
		}());
		DateTools.DateLocalization = DateLocalization;
		/**
        * Handles Conversions and Mainipulations of Date Properties.
        */
		var DateUtility = (function ()
		{
			function DateUtility()
			{
			}
			/**
            * Converts a local date object to it's UTC representation
            * @param	date - date to convert to UTC
            */
			DateUtility.ConvertDateToUTC = function (date)
			{
				var result = Date.UTC(date.getFullYear(), date.getMonth(), date.getDate(), date.getHours(), date.getMinutes(), date.getSeconds(), date.getMilliseconds());
				return result;
			};
			/**
             * Converts a DateTime that has already been augmented by a timezone offset (not native to javascript) to UTC
             * @param date - date to convert to UTC
             * @param zone - numeric timezone offset between UTC -12 to UTC +12
             */
			DateUtility.ConvertDateToUTCFromTimeZone = function (date, zone)
			{
				// restrict timezone between -12 and +12.
				if (zone < -12)
				{
					zone = -12;
				}
				else if (zone > 12)
				{
					zone = 12;
				}
				var timezoneOffsetMinutes = zone * 60;
				var javascriptTimezoneOffsetMinutes = date.getTimezoneOffset();
				var finalTimeZoneOffset = javascriptTimezoneOffsetMinutes - timezoneOffsetMinutes;
				var result = new Date(DateUtility.ConvertDateToUTC(date));
				result.setMinutes(result.getMinutes() - finalTimeZoneOffset);
				return result;
			};
			/**
            * Converts a UTC date to it's local date.
            * @param	utcDate - UTC date to convert to normal local date.
            */
			DateUtility.ConvertDateToLocal = function (utcDate)
			{
				var result = new Date(utcDate.getTime());
				result.setMinutes(result.getMinutes() + result.getTimezoneOffset());
				return result;
			};
			/**
             * Converts a UTC date to a specific timezone offsete
             * @param utcDate - UTC date to convert to a local time zone
             * @param zone - number of hours to offset the timezone by usually between UTC -12 to UTC +12.
             */
			DateUtility.ConvertDateToTimeZone = function (utcDate, zone)
			{
				// restrict timezone between -12 and +12.
				if (zone < -12)
				{
					zone = -12;
				}
				else if (zone > 12)
				{
					zone = 12;
				}
				var timezoneOffsetMinutes = zone * 60; // convert the zone hours offset into minutes.
				var result = new Date(utcDate.getTime());
				result.setMinutes(result.getMinutes() + timezoneOffsetMinutes);
				return result;
			};
			/**
            * Get the number of days between the start date and the end date
            * @param	startDate - earliest date to start at
            * @param	endDate	 - latest date to calculate from.
            * @return	number of days between the start and end date.
            */
			DateUtility.GetDaysBetweenDates = function (startDate, endDate)
			{
				var start = DateUtility.ConvertDateToUTC(startDate);
				var end = DateUtility.ConvertDateToUTC(endDate);
				var daysInMiliseconds = end - start;
				return daysInMiliseconds / DateUtility.MillisecondsPerDay;
			};
			/**
            * Adds a number of days to the supplied date
            * @param	date - date to start at
            * @param	days - number of days to add (can be positive or negative).
            * @return	new date with added days.
            */
			DateUtility.AddDaysToDate = function (date, days)
			{
				var newDate = new Date(date.getTime());
				// NOTE: Rather than using the convert to UTC, simply directly add days to the current date.
				//var newDate = DateUtility.ConvertDateToUTC(date);//new Date(Date.UTC(date.getFullYear(), date.getMonth(), date.getDay(), date.getHours(), date.getMinutes(), date.getSeconds(), date.getMilliseconds()));
				newDate.setDate(newDate.getDate() + days);
				return newDate;
			};
			/**
            * Adds a number of weeks to the supplied date
            * @param	date - date to start at
            * @param	weeks - number of weeks to add (can be positive or negative).
            * @return	new date with added weeks.
            */
			DateUtility.AddWeeksToDate = function (date, weeks)
			{
				return DateUtility.AddDaysToDate(date, weeks * 7);
			};
			/**
            * Adds a number of months to the supplied date
            * @param	date - date to start at
            * @param	month - number of months to add (can be positive or negative).
            * @return	new date with added months.
            */
			DateUtility.AddMonthsToDate = function (date, months)
			{
				var years = Math.floor(months / 12);
				months = months % 12;
				return new Date(date.getFullYear() + years, date.getMonth() + months, date.getDate(), date.getHours(), date.getMinutes(), date.getSeconds(), date.getMilliseconds());
			};
			/**
            * Get the total number of days within a month for a given date
            * @param	date - date of the month to retrieve for
            * @return	number of days within that month.
            */
			DateUtility.GetDaysInMonthForDate = function (date)
			{
				var endDate = DateUtility.AddDaysToDate(new Date(date.getFullYear(), date.getMonth() + 1), -1);
				return endDate.getDate();
			};
			/**
            * return the Start of the month from a given date
            * @param	date - date to get the start of the month for.
            */
			DateUtility.GetMonthStart = function (date)
			{
				return new Date(date.getFullYear(), date.getMonth(), 1);
			};
			/**
            * Return the End of the Month from a given date
            * @param	date - date to get the end of the month for.
            */
			DateUtility.GetMonthEnd = function (date)
			{
				return new Date(date.getFullYear(), date.getMonth(), DateUtility.GetDaysInMonthForDate(date));
			};
			/**
            * Get the total number of days within a Quarter for a given date
            * @param	date - date of the Quarter to retrieve for
            * @return	number of days within that Quarter.
            */
			DateUtility.GetDaysInQuarterForDate = function (date)
			{
				var quarterStart = this.GetQuarterStartDateForDate(date);
				var daysInMonth = this.GetDaysInMonthForDate(date);
				daysInMonth += this.GetDaysInMonthForDate(this.AddMonthsToDate(date, 1));
				daysInMonth += this.GetDaysInMonthForDate(this.AddMonthsToDate(date, 2));
				return daysInMonth;
			};
			/**
            * Get the Quarter for which the month belongs in.
            * @param	month - to check
            * @return	the quarter in which the month belongs (0 - 3)
            */
			DateUtility.GetQuarterForMonth = function (month)
			{
				// First Quarter Janurary-March
				if (month >= 0 && month <= 2)
				{
					return 0;
				}
				else if (month >= 3 && month <= 5)
				{
					return 1;
				}
				else if (month >= 6 && month <= 8)
				{
					return 2;
				}
				else if (month >= 9 && month <= 11)
				{
					return 3;
				}
				return -1;
			};
			/**
            * Returns the start date of the quarter for the supplied date
            * @param	date - date to look for the quarter for.
            * @return	start date for the quarter that the supplied date belongs to.
            */
			DateUtility.GetQuarterStartDateForDate = function (date)
			{
				var quarter = DateUtility.GetQuarterForMonth(date.getMonth());
				return new Date(date.getFullYear(), quarter * 3);
			};
			/**
            * Returns the end date of the quarter for the supplied date
            * @param	date - date to look for the quarter for.
            * @return	end date for the quarter that the supplied date belongs to.
            */
			DateUtility.GetQuarterEndDateForDate = function (date)
			{
				var quarter = DateUtility.GetQuarterForMonth(date.getMonth());
				return new Date(date.getFullYear(), quarter * 3 + 2);
			};
			/**
            * Returns the total number of days within the given year of the date
            * @param	year - year to calculate the total days for.
            */
			DateUtility.GetDaysInYear = function (year)
			{
				var startDate = DateUtility.GetYearStartForDate(year);
				var endDate = DateUtility.GetYearEndForDate(year);
				return DateUtility.GetDaysBetweenDates(startDate, endDate);
			};
			/**
            * Get the first date of the year
            * @param	year - the year to get the first day for.
            */
			DateUtility.GetYearStartForDate = function (year)
			{
				return new Date(year, 0, 1);
			};
			/**
            * Get the last date of the year
            * @param	year - the year to get the last day for.
            */
			DateUtility.GetYearEndForDate = function (year)
			{
				return new Date(year, 11, 31);
			};
			/** Result of 24 hours * 60 minutes * 60 seconds * 1000 miliseconds */
			DateUtility.MillisecondsPerDay = 86400000;
			return DateUtility;
		}());
		DateTools.DateUtility = DateUtility;
	})(DateTools = pi.DateTools || (pi.DateTools = {}));
})(pi || (pi = {}));

(function (pi)
{
	/**
     * This section of the UI module deals exclusively with automating the form system.
     */
	var Rest;
	(function (Rest)
	{
		/**
         * The Client Factory can create factories known and even add clients unknown.
         */
		var ClientFactory = (function ()
		{
			function ClientFactory()
			{
			}
			/**
             * Retrieve a Client from the client manager list.
             * @param clientType
             */
			ClientFactory.GetClient = function (clientType)
			{
				if (ClientFactory.clients.HasItem(clientType))
				{
					return ClientFactory.clients.Get(clientType);
				}
				else
				{
					switch (clientType)
					{
						case "ProjectType":
							ClientFactory.AddClient(clientType, new pi.Rest.Client.ProjectTypeClient(pi.Rest.ApiRoot));
							return ClientFactory.clients.Get(clientType);
						case "Group":
							ClientFactory.AddClient(clientType, new pi.Rest.Client.GroupClient(pi.Rest.ApiRoot));
							return ClientFactory.clients.Get(clientType);
						default:
							if (ClientFactory.OnGetClient != null)
							{
								return ClientFactory.OnGetClient(clientType);
							}
					}
				}
				return null;
			};
			/**
             * Adds a Client to the hash table responsible for managing clients.
             * @param clientType - string representation of the client type.
             * @param client - client to be added
             */
			ClientFactory.AddClient = function (clientType, client)
			{
				if (!pi.JSTools.IsUndefinedOrNull(client))
				{
					ClientFactory.clients.Add(clientType, client);
				}
			};
			ClientFactory.clients = new pi.HashTable();
			ClientFactory.OnGetClient = null;
			return ClientFactory;
		}());
		Rest.ClientFactory = ClientFactory;
	})(Rest = pi.Rest || (pi.Rest = {}));
})(pi || (pi = {}));

(function (pi)
{
	var Rest;
	(function (Rest)
	{
		/**
         * This class represents a rest response from the server.
         */
		var RestResponse = (function ()
		{
			/**
             * Create an instance of the Rest Response object.
             * @param config
             */
			function RestResponse(config)
			{
				this.SaveId = null;
				this.Success = false;
				this.Json = null;
				this.Errors = [];
				this.Status = "";
				if (!pi.JSTools.IsUndefinedOrNull(config))
				{
					this.Success = config.Success;
					this.Json = config.Json;
				}
			}
			return RestResponse;
		}());
		Rest.RestResponse = RestResponse;
		/**
         * Base Class for API Clients.
         */
		var ClientBase = (function ()
		{
			/**
             * Construct an instance of the Client Base object.
             * @param base - optional parameter to supply the root url to the client.
             */
			function ClientBase(base)
			{
				this.BaseUrl = "";
				if (!pi.JSTools.IsUndefinedNullOrEmpty(base))
				{
					this.BaseUrl = base;
				}
				this.urlParameters = null;

				this.TimeoutHandle = null;
				this.OnElementClicked = null;
				this.OnDisableButton = null;
				this.OnEnableDisabledElement = null;

				this.DisabledElement = null;
				this.SpinnerElementId = null;
				this.ClickedElement = null;
			}
			/**
             * Helper function to generate the API URL.
             * @param apiTarget - non-root target to the API functionality to call
             * @param id - id to append to the url if requesting an object
             * @param query - query json object
             * @param fields - specific fields to request
			 * @param ignoreBase - does not append the base URL.
             * @return string - builds out the url for rest responses.
             */
			ClientBase.prototype.GetURL = function (apiTarget, id, query, modelProperties, ignoreBase)
			{
				var url = this.BaseUrl + apiTarget;;
				if(!pi.JSTools.IsUndefinedOrNull(ignoreBase) && ignoreBase)
				{
					url = apiTarget;
				}
				if (!pi.JSTools.IsUndefinedNullOrEmpty(id))
				{
					url += "/" + id;
				}
				var queryString = "";
				if (pi.JSTools.IsUndefinedNullOrEmpty(query))
				{
					query = this.urlParameters;
					this.ClearParameters();
				}
				if (!pi.JSTools.IsUndefinedNullOrEmpty(query))
				{
					for (var property in query)
					{
						if (query.hasOwnProperty(property))
						{
							if (!pi.JSTools.IsUndefinedNullOrEmpty(queryString))
							{
								queryString += "&";
							}
							var value = query[property];
							if (pi.JSTools.IsString(value))
							{
								value = encodeURIComponent(value);
							}
							queryString += property + "=" + value.toString();
						}
					}
					url += "?" + queryString;
				}
				if (!pi.JSTools.IsUndefinedNullOrEmpty(modelProperties))
				{
					if (!pi.JSTools.IsUndefinedNullOrEmpty(queryString))
					{
						url += "&";
					}
					else
					{
						url += "?";
					}
					url += "modelProperties=" + modelProperties; //encodeURIComponent(modelProperties);
				}
				return url;
			};
			/**
             * Get the Ajax Settings to perform an Ajax operation.
             * @param url - url to the API to call.
             * @param method - method of performing the API call
             * @param done - function to execute when done.
             * @param data - data to send to the API
             */
			ClientBase.prototype.GetAjaxSettings = function (url, method, done, data)
			{
				var self = this;
				var response = new RestResponse();
				var ajaxSettings = {};
				ajaxSettings.url = url;
				ajaxSettings.method = method;
				ajaxSettings.dataType = "json";
				ajaxSettings.data = data;
				ajaxSettings.success = function(json, statusText)
				{
					response.Success = true;
					response.Json = json;
					response.Status = status;
					if (!pi.JSTools.IsUndefinedNullOrEmpty(json))
					{
						// if the returned json contains an errors array then we are dealing with an API response object.
						if (!pi.JSTools.IsUndefinedOrNull(json.Errors))
						{
							if (!json.Success)
							{
								response.Success = json.Success;
								response.Json = null;
							}
							response.Errors = json.Errors;
						}
					}
					else
					{
						// Assume the action was a success
						response.Success = true;
					}
					// Call the universal done function supplied by the application.
					if (!pi.JSTools.IsUndefinedOrNull(self.OnSuccess))
					{
						self.OnSuccess(response);
					}
					// Call the Done function supplied by the application.
					if (!pi.JSTools.IsUndefinedOrNull(done))
					{
						done(response);
					}
					else
					{
						// if no done function supplied call the default handler.
						pi.Rest.DefaultResponseHandler(response);
					}
					// Call general After Ajax delegate
					if (!pi.JSTools.IsUndefinedOrNull(self.OnAfterAjax))
					{
						self.OnAfterAjax(response);
					}
				};
				ajaxSettings.error = function(xhr, status, errorThrown)
				{
					response.Success = false;
					response.Json = null;
					response.Status = status;
					// This is usually an error from the ajax system itself.
					response.Errors.push({
						Model: null,
						SDK: null,
						Property: "",
						AdditionalInfo: xhr.responseText,
						ErrorMessage: errorThrown,
						ApiErrorType: 0
					});
					// Call the error for the client.
					if (!pi.JSTools.IsUndefinedOrNull(self.OnError))
					{
						self.OnError(response);
					}
					// Call the Done function supplied by the application.
					if (!pi.JSTools.IsUndefinedOrNull(done))
					{
						done(response);
					}
					else
					{
						// if no supplied function call the default handler.
						pi.Rest.DefaultResponseHandler(response);
					}
					// Call general After Ajax delegate
					if (!pi.JSTools.IsUndefinedOrNull(self.OnAfterAjax))
					{
						self.OnAfterAjax(response);
					}
				}
				
				// call just before the ajax operations is fired.  This is technically a "side effect".
				if (!pi.JSTools.IsUndefinedOrNull(this.OnBeforeAjax))
				{
					this.OnBeforeAjax(ajaxSettings);
				}
				return ajaxSettings;
			};
			/**
             * Get data from the API
             * @param apiTarget - string uri to the rest operation you want to call.
             * @param id - id of the object you want to get, null or empty if not applicable
             * @param done - function to call when the command finishes, contains the response object as function parameter
             * @param query - json object that will be transformed into the query string
             * @param modelProperties - specified model properties to attach to the query string to get specific fields.
			 * @param {any} clickedElementId - element Id to set the spinner next to
			 * @param {any} disableElementId - element id to disable when ajax is sent.
			 * @param {number} offsetX - offset of the spinner element in X pixels
			 * @param {number} offsetY - offset of the spinner element in Y Pixels
             */
			ClientBase.prototype.get = function (apiTarget, id, done, query, modelProperties, clickedElementId, disableElementId, offsetX, offsetY)
			{
				this.InitializeAjaxCall(clickedElementId, disableElementId, offsetX, offsetY);

				var url = this.GetURL(apiTarget, id, query, modelProperties);
				$.ajax(this.GetAjaxSettings(url, "get", done));
			};
			/**
             * Post Data to the API
             * @param apiTarget - string uri to the rest operation you want to call.
             * @param data - data to post to the server
             * @param done - function to utilize when successful.
             * @param query - json object that will be transformed into the query string
			 * @param {any} clickedElementId - element Id to set the spinner next to
			 * @param {any} disableElementId - element id to disable when ajax is sent.
			 * @param {number} offsetX - offset of the spinner element in X pixels
			 * @param {number} offsetY - offset of the spinner element in Y Pixels
             */
			ClientBase.prototype.post = function (apiTarget, data, done, query, clickedElementId, disableElementId, offsetX, offsetY)
			{
				this.InitializeAjaxCall(clickedElementId, disableElementId, offsetX, offsetY);

				var url = this.GetURL(apiTarget, null, query);
				$.ajax(this.GetAjaxSettings(url, "post", done, data));
			};
			/**
             * Delete objects from the API
             * @param apiTarget - string uri to the rest operation you want to call.
             * @param id - id of the object you want to get, null or empty if not applicable
             * @param done - function to utilize when successful.
             * @param query - json object that will be transformed into the query string
			 * @param {any} clickedElementId - element Id to set the spinner next to
			 * @param {any} disableElementId - element id to disable when ajax is sent.
			 * @param {number} offsetX - offset of the spinner element in X pixels
			 * @param {number} offsetY - offset of the spinner element in Y Pixels
             */
			ClientBase.prototype.delete = function (apiTarget, id, done, query, clickedElementId, disableElementId, offsetX, offsetY)
			{
				this.InitializeAjaxCall(clickedElementId, disableElementId, offsetX, offsetY);

				var url = this.GetURL(apiTarget, id, query);
				$.ajax(this.GetAjaxSettings(url, "delete", done));
			};
			/**
             * Put objects from the API
             * @param apiTarget - string uri to the rest operation you want to call
			 * @param id - id of the object you want to get, null or empty if not applicable
             * @param data - data to put to the server
             * @param done - function to call when finished witht he ajax call.
             * @param query - json object that will be transformed into the query string
			 * @param {any} clickedElementId - element Id to set the spinner next to
			 * @param {any} disableElementId - element id to disable when ajax is sent.
			 * @param {number} offsetX - offset of the spinner element in X pixels
			 * @param {number} offsetY - offset of the spinner element in Y Pixels
             */
			ClientBase.prototype.put = function (apiTarget, id, data, done, query, clickedElementId, disableElementId, offsetX, offsetY)
			{
				var url = this.GetURL(apiTarget, id, query);
				$.ajax(this.GetAjaxSettings(url, "put", done));
			};
			/**
            * Get a specific Project Type.
            * @virtual
            * @abstract
            * @param id - id of the object to get
            * @param done - call back function when the ajax returns
            * @param query - json object that will be transformed into the query string
            * @param modelProperties - json properties to request the system to return.
            * @remark Override in base class.  See ProjectTypeClient for examples
            */
			ClientBase.prototype.Get = function (id, done, query, modelProperties)
			{
				// override in base class.  See ProjectStatusClient for example
				// this.get("/project-type", id, done, modelProperties);
			};
			/**
             * Save the properties of the specified Project Type.
             * @param restObject - object to be saved to.
             * @param done - call back function when the ajax returns
             * @param query - json object that will be transformed into the query string
             * @remark Override in base class.  See ProjectTypeClient for examples
             */
			ClientBase.prototype.Save = function (restObject, done, query)
			{
				// override in base class.  See ProjectTypeClient for example
				// this.post("/project-type", restObject, done);
			};
			/**
             * Delete a Project Type from the System.
             * @param id - id of the object to delete
             * @param done - call back function when the ajax returns
             * @remark Override in base class.  See ProjectTypeClient for examples
             */
			ClientBase.prototype.Delete = function (id, done)
			{
				// override in base class.  See ProjectTypeClient for example
				// this.delete("/project-type", id, done);
			};
			/**
             * Adds a Parameter to the next url request
             * @param name - name of the parameter
             * @param value - value of the parameter
             */
			ClientBase.prototype.AddParameter = function (name, value)
			{
				if (pi.JSTools.IsUndefinedNullOrEmpty(this.urlParameters))
				{
					this.urlParameters = {};
				}
				this.urlParameters[name] = value;
			};
			/**
             * Clears the parameters in the added URL Parameters object
             */
			ClientBase.prototype.ClearParameters = function ()
			{
				this.urlParameters = null;
			};

			/** 
			* Called before an Ajax execution to setup the spinner and disable button elements.
			* @param {any} clickedElementId - element id to set the spinner next to
			* @param {any} disableElementId - element id to disable when ajax is sent
			* @param {number} offsetX - offset of the spinner element in X pixels
			* @param {number} offsetY - offset of the spinner element in Y Pixels
			* @private
			*/
			ClientBase.prototype.InitializeAjaxCall = function (clickedElementId, disableElementId, offsetX, offsetY)
			{
				if (!pi.JSTools.IsUndefinedNullOrEmpty(clickedElementId))
				{
					this.ClickedElement = GetElementById(clickedElementId);
				}

				if (!pi.JSTools.IsUndefinedNullOrEmpty(clickedElementId))
				{
					this.DisabledElement = GetElementById(clickedElementId);
				}

				if (pi.JSTools.IsUndefinedNullOrEmpty(offsetX))
				{
					offsetX = 0;
				}

				if (pi.JSTools.IsUndefinedNullOrEmpty(offsetY))
				{
					offsetY = 0;
				}

				this.SetupSpinner();
				this.OnFormElementClicked(offsetX, offsetY);
				this.OnFormDisableButton();
			};

			/** 
			* Called when a Form Element has been Clicked
			* @param {number} offsetX - offset of the spinner element in X pixels
			* @param {number} offsetY - offset of the spinner element in Y Pixels
			* @protected
			*/
			ClientBase.prototype.OnFormElementClicked = function (offsetX, offsetY)
			{
				if (this.OnElementClicked !== null)
				{
					this.OnElementClicked(this.ClickedElement, offsetX, offsetY);
				}
				else
				{
					this.OnElementClickedDefault(offsetX, offsetY);
				}
			};

			/** 
			* Called when a form element is to be disabled
			* @protected
			*/
			ClientBase.prototype.OnFormDisableButton = function ()
			{
				if (this.OnDisableButton !== null)
				{
					this.OnDisableButton(this.DisabledElement);
				}
				else
				{
					this.OnDisableButtonDefault();
				}
			};

			/** 
			* Called to reenable an element that was disabled.
			* @protected
			*/
			ClientBase.prototype.EnableDisabledElement = function ()
			{
				if (this.OnEnableDisabledElement !== null)
				{
					this.OnEnableDisabledElement(this.DisabledElement);
				}
				else
				{
					this.OnEnableDisabledElementDefault();
				}
			};

			/** 
			* Called if an Element has been clicked and sent in to the get method.
			* @param {number} offsetX - offset of the spinner element in X pixels
			* @param {number} offsetY - offset of the spinner element in Y Pixels
			* @private
			*/
			ClientBase.prototype.OnElementClickedDefault = function (offsetX, offsetY)
			{
				if (this.ClickedElement !== null)
				{
					var x = UITools.GetPageXCoordinate(this.ClickedElement) + offsetX;
					var y = UITools.GetPageYCoordinate(this.ClickedElement) + offsetY;
					this.ShowSpinner(x, y);
				}
			};

			/** 
			* Called as the default event to disable an Element
			* @private
			*/
			ClientBase.prototype.OnDisableButtonDefault = function ()
			{
				if (this.DisabledElement !== null)
				{
					UITools.DisableElement(this.DisabledElement);
				}
			};

			/** 
			* Called to reenable an element that was disabled.
			* @private
			*/
			ClientBase.prototype.OnEnableDisabledElementDefault = function ()
			{
				if (this.DisabledElement !== null)
				{
					UITools.EnableElement(this.DisabledElement);
				}
			};

			/** 
			* Creates and SEtups up the Ajax Spinner used for the client.
			* @protected
			*/
			ClientBase.prototype.SetupSpinner = function ()
			{
				if (this.SpinnerElementId === null)
				{
					this.SpinnerElementId = "clientProcessingDiv";
					var divHTML = "<div id=\"" + this.SpinnerElementId + "\" class=\"AjaxLoadingDiv\" style=\"position: absolute; z-index: 30000; display:none;\">" + "</div>";
					InsertHTML(divHTML);
				}

			};

			/** 
			* Show the spinner at the specified x, y location
			* @param {number} x - x pixel coordinate to show the spinner at
			* @param  {number} y - y pixel coordinate to show the spinner at
			* @protected
			*/
			ClientBase.prototype.ShowSpinner = function (x, y)
			{
				UITools.ShowAtXY(x, y, this.SpinnerElementId, false, "", 0, 0, false, false, false);
			};

			/** 
			* Hide the spinner if it is currently being displayed
			* @protected
			*/
			ClientBase.prototype.HideSpinner = function ()
			{
				if (!pi.JSTools.IsUndefinedNullOrEmpty(this.SpinnerElementId))
				{
					UITools.Hide(this.SpinnerElementId, "");
				}
			};

			/** 
			* Called when an Ajax function finishes completes to take care of the spinner
			* @param {object} response - the ajax response object
			* @protected
			* @virtual
			*/
			ClientBase.prototype.OnAfterAjax = function (response)
			{
				this.HideSpinner();
				this.ClickedElement = null;

				this.EnableDisabledElement();
				this.DisabledElement = null;
			};


			return ClientBase;
		}());
		Rest.ClientBase = ClientBase;
	})(Rest = pi.Rest || (pi.Rest = {}));
})(pi || (pi = {}));

(function (pi)
{
	var Rest;
	(function (Rest)
	{
		var Client;
		(function (Client)
		{
			var LocalStorageClient = (function (_super)
			{
				__extends(LocalStorageClient, _super);
				/**
                 * Create an instance of the Group object.
                 * @param apiBase
                 */
				function LocalStorageClient(apiBase)
				{
					_super.call(this, apiBase);
				}
				/**
                 * Gets the value from local storage based on the key name
                 * @param name - key in local storage
                 * @param done - callback function to execute when finished
                 * @param modelProperties - additional properties to include in the result.
                 * @param expires - length of time in seconds before cache expires.
                 */
				LocalStorageClient.prototype.get = function (name, id, done, query, modelProperties, expires)
				{
					//this.get(name, null, done, null, modelProperties);
					var cacheName = this.GetURL(name, id, query, modelProperties, false);
					if (cacheName.indexOf(pi.Root) < 0)
					{
						cacheName = pi.Root + name;
					}
					var info = pi.UI.LocalStorage.Get(cacheName, done, modelProperties);
					if (info == "")
					{
						_super.prototype.get.call(this, cacheName, id, function (data)
						{
							info = JSON.stringify(data.Json);
							pi.UI.LocalStorage.Set(cacheName, JSON.stringify(data.Json), expires);
							if(!pi.JSTools.IsUndefinedOrNull(done))
							{
								done(data);
							}
						}, query, modelProperties);
					}
					else
					{
						var response = new pi.Rest.RestResponse();
						response.Success = true;
						response.Json = info;

						// call ajax done function.
						if(!PI.JSTools.IsUndefinedOrNull(done))
						{
							done(response);
						}

						// Call general After Ajax delegate
						if (!pi.JSTools.IsUndefinedOrNull(this.OnAfterAjax))
						{
							this.OnAfterAjax(response);
						}
					}
					return info;
				};
				return LocalStorageClient;
			}(pi.Rest.ClientBase));
			Client.LocalStorageClient = LocalStorageClient;
		})(Client = Rest.Client || (Rest.Client = {}));
	})(Rest = pi.Rest || (pi.Rest = {}));
})(pi || (pi = {}));

(function (pi)
{
	var Rest;
	(function (Rest)
	{
		var MetafuseClient = (function (_super)
		{
			__extends(MetafuseClient, _super);
			/**
             * Create an instance of the MetafuseClient
             * @param base
             */
			function MetafuseClient(base)
			{
				_super.call(this, base);
			}
			/**
             * Called to setup the user's display just before invoking an Ajax Call.
             * @param ajaxSettings - The ajax settings object constructed to perform the ajax call.
             */
			MetafuseClient.prototype.BeforeAjax = function (ajaxSettings)
			{
				// Start Spinner
			};
			/**
             * Called to tear down any special display after an ajax call successful or failed.
             * @param response - Response of the ajax call.
             */
			MetafuseClient.prototype.AfterAjax = function (response)
			{
				// Stop Spinner
			};
			return MetafuseClient;
		}(pi.Rest.Client.LocalStorageClient));
		Rest.MetafuseClient = MetafuseClient;
	})(Rest = pi.Rest || (pi.Rest = {}));
})(pi || (pi = {}));

(function (pi)
{
	var Rest;
	(function (Rest)
	{
		var Client;
		(function (Client)
		{
			var GroupClient = (function (_super)
			{
				__extends(GroupClient, _super);
				/**
                 * Create an instance of the Group object.
                 * @param apiBase
                 */
				function GroupClient(apiBase)
				{
					_super.call(this, apiBase);
				}
				/**
                 * Get List of Groups that the you can communicate with.
                 * @param done - callback function to execute when finished.
                 * @param modelProperties - additional model properties to add
                 */
				GroupClient.prototype.ListCommunicate = function (done, modelProperties)
				{
					this.get("/group/list-communicate", null, done, modelProperties);
				};
				/**
                 * Get a list of all groups
                 * @param done - callback function to execute when finished
                 * @param modelProperties - additional properties to include in the result.
                 */
				GroupClient.prototype.ListAll = function (done, modelProperties)
				{
					this.get("/group/list-all", null, done, modelProperties);
				};
				/**
                 * Get a list of groups
                 * @param done - callback function to execute when finished
                 * @param modelProperties - additional properties to include in the result.
                 */
				GroupClient.prototype.List = function (done, modelProperties)
				{
					this.get("/group/list", null, done, modelProperties);
				};
				/**
                 * Get a specific group by it's ID
                 * @param id - id of the group to retrieve
                 * @param done - callback function to execute when finished
                 * @param modelProperties - additional properties to include in the result.
                 */
				GroupClient.prototype.Get = function (id, done, query, modelProperties)
				{
					this.get("/group", id, done, query, modelProperties);
				};
				/**
                 * Create or Update a group object
                 * @param group - JSON data to push that will create or update the group
                 * @param done - callback function to execute when finished.
                 */
				GroupClient.prototype.Save = function (group, done)
				{
					this.post("/group", group, done);
				};
				/**
                 * Deletes a group from the system
                 * @param id - id of the group of delete
                 * @param done - callback function to execute when finished.
                 */
				GroupClient.prototype.Delete = function (id, done)
				{
					this.delete("/group", id, done);
				};
				return GroupClient;
			}(Rest.MetafuseClient));
			Client.GroupClient = GroupClient;
		})(Client = Rest.Client || (Rest.Client = {}));
	})(Rest = pi.Rest || (pi.Rest = {}));
})(pi || (pi = {}));

(function (pi)
{
	var Rest;
	(function (Rest)
	{
		var Client;
		(function (Client)
		{
			/**
             * Javascript Client for Project Types.
             */
			var ProjectTypeClient = (function (_super)
			{
				__extends(ProjectTypeClient, _super);
				/**
                 * Create an instance of the ProjectTypeClient object.
                 * @param apiBase
                 */
				function ProjectTypeClient(apiBase)
				{
					_super.call(this, apiBase);
				}
				/**
                 * Get all Project Types from the API
                 * @param fields
                 */
				ProjectTypeClient.prototype.ListAll = function (done, modelProperties)
				{
					this.get("/project-type/list-all", null, done, modelProperties);
				};
				/**
                 * Get all Project Types that are active.
                 * @param fields
                 */
				ProjectTypeClient.prototype.ListActive = function (done, modelProperties)
				{
					this.get("/project-type/list-active", null, done, modelProperties);
				};
				/**
                 * Get a specific Project Type.
                 * @param id
                 * @param fields
                 */
				ProjectTypeClient.prototype.Get = function (id, done, modelProperties)
				{
					this.get("/project-type", id, done, modelProperties);
				};
				/**
                 * Save the properties of the specified Project Type.
                 * @param restObject - object to be saved to.
                 * @param done - call back function when the ajax returns
                 * @param query - json object that will be transformed into the query string
                 * @remark Override in base class.  See ProjectTypeClient for examples
                 */
				ProjectTypeClient.prototype.Save = function (restObject, done, query)
				{
					this.post("/project-type", restObject, done);
				};
				/**
                 * Delete a Project Type from the System.
                 * @param id
                 * @param done
                 */
				ProjectTypeClient.prototype.Delete = function (id, done)
				{
					this.delete("/project-type", id, done);
				};
				return ProjectTypeClient;
			}(Rest.MetafuseClient));
			Client.ProjectTypeClient = ProjectTypeClient;
		})(Client = Rest.Client || (Rest.Client = {}));
	})(Rest = pi.Rest || (pi.Rest = {}));
})(pi || (pi = {}));

(function (pi)
{
	var UI;
	(function (UI)
	{
		/**
         * This class is responsible for handeling browser specification and identification
         */
		var BrowserTools = (function ()
		{
			function BrowserTools()
			{
			}
			/**
             * Determines if the current browser is IE
             * @return - boolean flag indicating if the current browser is IE when true.
             */
			BrowserTools.IsInternetExplorer = function ()
			{
				if (navigator.userAgent.indexOf("MSIE") >= 0)
				{
					return true;
				}
				else if (navigator.userAgent.match("Trident\/7.0;.*rv:11.0"))
				{
					// IE 11
					return true;
				}
				return false;
			};
			/**
             * Determines if the current browser is Chrome
             * @return - boolean flag indicating if the current browser is Chrome when true.
             */
			BrowserTools.IsChrome = function ()
			{
				if (navigator.userAgent.toLowerCase().indexOf("chrome") > -1)
				{
					return true;
				}
				return false;
			};
			/**
             * detects if the current browser is Safari
             * @return - boolean flag indicating if the current browser is Safari when true.
             */
			BrowserTools.IsSafari = function ()
			{
				if (navigator.userAgent.toLowerCase().indexOf("safari") > -1 && this.IsChrome() == false)
				{
					return true;
				}
				return false;
			};
			/**
             * Determines if the current browser is Firefox
             * @return - boolean flag indicating if the current browser is Firefox when true.
             */
			BrowserTools.IsFirefox = function ()
			{
				if (navigator.userAgent.indexOf("Firefox") > -1)
				{
					return true;
				}
				return false;
			};
			/**
             * Determine if the current browser is Netscape
             * @return - boolean flag indicating if the current browser is netscape when true
             * @remark - we may not even need this anymore.
             */
			BrowserTools.IsNetscape = function ()
			{
				if (!BrowserTools.IsSafari() && !BrowserTools.IsFirefox())
				{
					if (navigator.appName.toLowerCase().indexOf("netscape") > -1)
					{
						return true;
					}
				}
				return false;
			};
			/**
             * Get the major version of the browser
             * @return - number indicating the major version.
             */
			BrowserTools.GetMajorVersion = function ()
			{
				var userAgent = navigator.userAgent;
				if (BrowserTools.IsInternetExplorer())
				{
					var verOffset = userAgent.indexOf("MSIE");
					return parseFloat(userAgent.substring(verOffset + 5, userAgent.length));
				}
				else
				{
					// TODO: Get rid of this alert object.
					alert("Unsupported for the browser");
				}
				return parseFloat(navigator.appVersion);
			};
			/**
             * Determines if IE browser is zoomed
             * @return true if the IE browser's Y DPI is not equal to 96
             * @remark this may no longer be true as the original function seemed to only care about IE 8 and IE 7... Plus DPI settings can differ.
             */
			BrowserTools.IsIEBrowserZoomed = function ()
			{
				var isZoomed = false;
				if (BrowserTools.IsInternetExplorer())
				{
					isZoomed = (window.screen.deviceYDPI != 96);
				}
				return isZoomed;
			};
			/**
             * Determines if a device is touch enabled or not.
             * @return - boolean flag indicating the device has touch when true.
             */
			BrowserTools.IsTouchEnabled = function ()
			{
				return ('ontouchstart' in window) || ('onmsgesturechange' in window);
			};
			/**
             * Determines if a device is specifically "TouchStart" enabled (IOS)
             * @return boolean flag indicating wheather or not the agent is touch enabled when true.
             */
			BrowserTools.IsTouchStartEnabled = function ()
			{
				return ('ontouchstart' in window);
			};
			BrowserTools.BrowserIsTabletDevice = false;
			BrowserTools.BrowserIsMobileDevice = false;
			return BrowserTools;
		}());
		UI.BrowserTools = BrowserTools;
	})(UI = pi.UI || (pi.UI = {}));
})(pi || (pi = {}));

(function (pi)
{
	/**
     * This section of the UI module deals exclusively with handeling the Draggable Layer System.
     */
	var UI;
	(function (UI)
	{
		/**
         * Represents a Dynamic Draggable Layer
         */
		var DraggableLayer = (function ()
		{
			/**
             * Construct an instance of the Draggable Layer object
             * @param config - optional Configuration object for this Layer
             */
			function DraggableLayer(config)
			{
				this.Id = "DraggableLayer";
				this.title = "Alert";
				this.message = "";
				this.width = null;
				this.Index = 0;
				this.HasCancelButton = true;
				this.Cancel = {
					JSClick: ""
				};
				this.Options = [];
				if (!pi.JSTools.IsUndefinedOrNull(config))
				{
					this.SetConfig(config);
				}
				this.RegisterButtonEvents();
			}
			/**
             * Set configuration values of the draggable layer object.
             * @param config - Configuration object for this Layer
             */
			DraggableLayer.prototype.SetConfig = function (config)
			{
				if (!pi.JSTools.IsUndefinedOrNull(config.Id))
				{
					this.Id = config.Id;
				}
				if (!pi.JSTools.IsUndefinedOrNull(config.Title))
				{
					this.title = config.Title;
				}
				if (!pi.JSTools.IsUndefinedOrNull(config.Message))
				{
					this.message = config.Message;
				}
				if (!pi.JSTools.IsUndefinedOrNull(config.Width))
				{
					this.width = config.Width;
				}
				if (!pi.JSTools.IsUndefinedOrNull(config.Cancel))
				{
					this.Cancel = config.Cancel;
				}
				if (!pi.JSTools.IsUndefinedOrNull(config.Options))
				{
					this.Options = config.Options;
				}
				if (!pi.JSTools.IsUndefinedOrNull(config.HasCancelButton))
				{
					this.HasCancelButton = config.HasCancelButton;
				}
			};
			/**
             * Register button events for the draggable layer
             */
			DraggableLayer.prototype.RegisterButtonEvents = function ()
			{
				if (this.Options.length > 0)
				{
					for (var i = 0; i < this.Options.length; ++i)
					{
						this.SetupButtonEvent(this.Options[i]);
					}
				}
			};
			/**
             * Setup the button events for each button by defining the delegates that are fired when they are called
             * @param button - Button to setup the event delegate for.
             */
			DraggableLayer.prototype.SetupButtonEvent = function (button)
			{
				var self = this;
				if (pi.JSTools.IsUndefinedNullOrEmpty(button.OnClick))
				{
					if (!pi.JSTools.IsUndefinedNullOrEmpty(button.JSClick))
					{
						button.OnClick = function (e)
						{
							return eval(button.JSClick);
						};
					}
					else
					{
						button.OnClick = function (e)
						{
							UITools.Hide(self.Id, "");
							return false;
						};
					}
				}
				if (pi.JSTools.IsUndefinedNullOrEmpty(button.OnKeyDown))
				{
					if (!pi.JSTools.IsUndefinedNullOrEmpty(button.JSKeyDown))
					{
						button.OnKeyDown = function (e)
						{
							return eval(button.JSKeyDown);
						};
					}
				}
			};
			/**
             * Sets the Title of the Draggable lyaer
             * @param title
             */
			DraggableLayer.prototype.SetTitle = function (title)
			{
				this.title = title;
				this.Update();
			};
			/**
             * Sets the Message of the Draggable layer
             * @param message
             */
			DraggableLayer.prototype.SetBody = function (message)
			{
				this.message = message;
				this.Update();
			};
			/**
             * Appends additional information to the Body of the layer.
             * @param message
             */
			DraggableLayer.prototype.AppendBody = function (message)
			{
				this.message += message;
				this.Update();
			};
			/**
             * Set the Options array for a collection of button configurations.
             * @param buttons
             */
			DraggableLayer.prototype.SetOptions = function (buttons)
			{
				if (pi.JSTools.IsUndefinedOrNull(buttons))
				{
					this.Options = [];
				}
				else
				{
					this.Options = buttons;
				}
				this.Update();
			};
			/**
             * Add a Button to this Draggable layer.
             * @param button
             */
			DraggableLayer.prototype.AddButton = function (button)
			{
				this.Options.push(button);
				this.Update();
			};
			/**
             * Update the entire Draggable layer system.
             */
			DraggableLayer.prototype.Update = function ()
			{
				$("[data-pi-draggable-title='" + this.Id + "']").html(this.title);
				$("[data-pi-draggable-body='" + this.Id + "']").html(this.RenderBody());
				this.RegisterButtonEvents();
				$("[data-pi-draggable-options='" + this.Id + "']").html(this.RenderOptions());
			};
			/**
             * Get a String that can be used to directly access the layer via a click or mouse over event and call functions on it.
             * @return	string - string that can be used to directly access the layer as a variable to fire various DOM events.
             */
			DraggableLayer.prototype.GetDOMEventString = function ()
			{
				return 'pi.UI.DraggableLayerManager.GetLayer("' + this.Id + '")';
			};
			/**
             * Get the HTMLString that composes the overall structure of this Draggable Layer object.
             * @return	string - string that contains the HTML of this Draggable layer to be inserted into the page.
             * @remark This function can be overriden in sub classes to change it's behavior.
             */
			DraggableLayer.prototype.GetHtmlString = function ()
			{
				var tableWidthString = "";
				var cellWidthString = "";
				if (this.width != null && this.width > -1)
				{
					tableWidthString = 'width:' + this.width.toString() + 'px;';
					cellWidthString = ' style="' + tableWidthString + '"';
				}
				//else
				//{
				//	tableWidthString = 'max-width:50%;';
				//}
				var boxHtml = '<table id="' + this.Id + '" class="DraggableLayer" style="display:none;z-Index:9000;' + tableWidthString + ';" data-pi-draggable="' + this.Id + '">';
				boxHtml += '<tr onmousedown="UITools.DragElement(event,\'' + this.Id + '\')">';
				boxHtml += '<th id="' + this.Id + '_Title" class="DraggableLayer" data-pi-draggable-title="' + this.Id + '">' + this.title + '</th>';
				boxHtml += '<th class="DraggableLayer AR">';
				boxHtml += '<a href="javascript:;" onmousedown="UITools.CancelBubble(event);" class="DraggableLayerClose" onclick="UITools.Hide(\'' + this.Id + '\');' + this.Cancel.JSClick + ';">';
				boxHtml += '<img src="' + pi.Root + '/i/c.gif" alt="close" class="VM" />';
				boxHtml += '</a>';
				boxHtml += '</th>';
				boxHtml += '<tr><td id="' + this.Id + '_Body" class="DraggableLayer WLG" colspan="2" data-pi-draggable-body="' + this.Id + '"' + cellWidthString + '>';
				boxHtml += this.RenderBody();
				boxHtml += '</td></tr>';
				boxHtml += '<tr><td id="' + this.Id + '_Options" class="DraggableLayer AR" colspan="2" data-pi-draggable-options="' + this.Id + '">';
				boxHtml += this.RenderOptions();
				boxHtml += '</td></tr>';
				boxHtml += '</tr>';
				boxHtml += '</table>';
				return boxHtml;
			};
			/**
             * Render the body of this Draggable Layer
             */
			DraggableLayer.prototype.RenderBody = function ()
			{
				return this.message;
			};
			/**
             * Renders the Options to html string
             */
			DraggableLayer.prototype.RenderOptions = function ()
			{
				var buttons = "";
				for (var i = 0; i < this.Options.length; i++)
				{
					var button = "<a href='javascript:;' ";
					if (!pi.JSTools.IsUndefinedNullOrEmpty(this.Options[i].Id))
					{
						button += "id='" + this.Options[i].Id + "' ";
						button += "data-pi-draggable-button='" + this.Options[i].Id + "' ";
					}
					if (!pi.JSTools.IsUndefinedNullOrEmpty(this.Options[i].CSS))
					{
						button += "class='" + this.Options[i].CSS + "' ";
					}
					if (!pi.JSTools.IsUndefinedNullOrEmpty(this.Options[i].Style))
					{
						button += "style='" + this.Options[i].Style + "' ";
					}
					if (!pi.JSTools.IsUndefinedNullOrEmpty(this.Options[i].JSKeyDown))
					{
						button += "onkeydown='" + this.Options[i].JSKeyDown + "' ";
					}
					else if (!pi.JSTools.IsUndefinedOrNull(this.Options[i].OnKeyDown))
					{
						button += "onkeypress='" + this.GetDOMEventString() + ".Options[" + i.toString() + "].OnKeyDown(event);'";
					}
					if (!pi.JSTools.IsUndefinedNullOrEmpty(this.Options[i].JSClick))
					{
						button += "onmousedown='" + this.GetDOMEventString() + ".Options[" + i.toString() + "].OnClick(event);'";
					}
					else if (!pi.JSTools.IsUndefinedOrNull(this.Options[i].OnClick))
					{
						button += "onmousedown='" + this.GetDOMEventString() + ".Options[" + i.toString() + "].OnClick(event);'";
					}
					button += ">";
					if (!pi.JSTools.IsUndefinedNullOrEmpty(this.Options[i].Text))
					{
						button += this.Options[i].Text;
					}
					else if (!pi.JSTools.IsUndefinedNullOrEmpty(this.Options[i].Img))
					{
						button += "<img src='" + this.Options[i].Img + "'/>";
					}
					button += "</a>";
					buttons += button;
				}
				return buttons;
			};
			/**
             * Completely removes the Draggable layer form the system and the DOM
             */
			DraggableLayer.prototype.Destroy = function ()
			{
				var dragElement = $("[pi-data-draggable='" + this.Id + "']").get()[0];
				dragElement.outerHTML = "";
				DraggableLayerManager.Remove(this.Id);
			};
			return DraggableLayer;
		}());
		UI.DraggableLayer = DraggableLayer;
		/**
         * Manages Dynamically created Draggable Layers within the system.
         */
		var DraggableLayerManager = (function ()
		{
			function DraggableLayerManager()
			{
			}
			/**
             * Adds an id of a draggable layer to the list
             * @param id - string id identifying the draggable layer you are looking for.
             */
			DraggableLayerManager.Add = function (layer)
			{
				layer.Index = DraggableLayerManager.DraggableLayerList.length;
				DraggableLayerManager.DraggableLayerHash.Add(layer.Id, layer);
				DraggableLayerManager.DraggableLayerList.push(layer);
			};
			/**
             * Checks of an id of a draggable layer exists
             * @param id - string id identifying the draggable layer you are looking for.
             */
			DraggableLayerManager.HasLayer = function (id)
			{
				//return id in DraggableLayerManager.DraggableLayerHash;//DraggableLayerManager.DraggableLayers.indexOf(id) > -1;
				return DraggableLayerManager.DraggableLayerHash.HasItem(id);
			};
			/**
             * Removes a draggable layer by the specified identifier.
             * @param id - string id of the draggable layer you want to remove.
             */
			DraggableLayerManager.Remove = function (id)
			{
				if (DraggableLayerManager.HasLayer(id))
				{
					// get the layer
					var layer = DraggableLayerManager.DraggableLayerHash.Get(id);
					// Remove the layer from the hash and the list
					DraggableLayerManager.DraggableLayerList.splice(DraggableLayerManager.DraggableLayerList.indexOf(layer), 1);
					DraggableLayerManager.DraggableLayerHash.Remove(id);
					// re-index the layer list.
					// could probably avoid this by having the layer itself call "index of" instead of tracking the index...
					for (var i = 0; i < DraggableLayerManager.DraggableLayerList.length; ++i)
					{
						DraggableLayerManager.DraggableLayerList[i].Index = i;
					}
				}
			};
			/**
             * Get the Draggable Layer by it's ID.
             * @param id
             */
			DraggableLayerManager.GetLayer = function (id)
			{
				if (DraggableLayerManager.HasLayer(id))
				{
					return DraggableLayerManager.DraggableLayerHash.Get(id);
				}
				return null;
			};
			/**
             * Helper function to create a layer name if one is not automatically supplied.
             * @param id
             * @param makeUnique
             */
			DraggableLayerManager.GetLayerName = function (id, makeUnique)
			{
				if (id === void 0) { id = null; }
				if (makeUnique === void 0) { makeUnique = false; }
				var date = new Date();
				if (pi.JSTools.IsUndefinedOrNull(id))
				{
					id = "DraggableLayer_" + new Date().valueOf().toString();
				}
				else if (makeUnique)
				{
					id = id + "_" + new Date().valueOf().toString();
				}
				return id;
			};
			/**
             * Adds a Draggable Layer to the Page
             * @param layer
             */
			DraggableLayerManager.AddLayerToPage = function (layer)
			{
				var appendLayer = true;
				if (UI.DraggableLayerManager.HasLayer(layer.Id))
				{
					// try and get the previous draggable layer by that ID.
					var layerElement = pi.UI.GetDataDashJQuery("data-pi-draggable", layer.Id).get()[0];
					// If for some reason this data has been removed skip and append it to the bottom of the page.
					if (!pi.JSTools.IsUndefinedNullOrEmpty(layerElement))
					{
						appendLayer = false;
						layerElement.outerHTML = layer.GetHtmlString();
						layer = UI.DraggableLayerManager.GetLayer(layer.Id);
					}
				}
				if (appendLayer)
				{
					UI.DraggableLayerManager.Add(layer);
					var $pageForm = $('form');

					if ($pageForm.length)
					{
						$pageForm.first().append(layer.GetHtmlString());
					}
					else
					{
						$('body').append(layer.GetHtmlString());
					}
				}
			};
			/**
             * Create a dynamic Draggable Layer on the page and return it's ID
             * @param id - optional string identifier of the Draggable layer.  if not specified will begin with "DraggableLayer"
             * @param makeUnique - optional flag indicating to make the id unique using a date timestamp string.  defaults to false unless no id is specified.
             * @param width - optional flag indicating the width of the draggable layer, must be greater than or equal to 450 pixels
             * @param removeOnClose - optional flag indicating to remove this draggable layer and all it's contents upon closing
             * @param onCloseJS - optional javascript to execute when the close function is called.  Must be properly formatted javascript string enclosed in single quotes.
             * @return string - id of the draggable layer that you can use to set the message body or the title
             */
			DraggableLayerManager.CreateDraggableLayer = function (id, makeUnique, width, removeOnClose, onCloseJS, isStrechable)
			{
				if (id === void 0) { id = null; }
				if (makeUnique === void 0) { makeUnique = false; }
				if (removeOnClose === void 0) { removeOnClose = false; }
				if (isStrechable === void 0) { isStrechable = false; }
				id = DraggableLayerManager.GetLayerName(id, makeUnique);
				var layer = null;
				if (UI.DraggableLayerManager.HasLayer(id))
				{
					layer = UI.DraggableLayerManager.GetLayer(id);
				}
				else
				{
					var layerConfig = {
						Id: id,
						Width: width,
						Cancel: {
							JSClick: onCloseJS
						}
					};
					layer = new DraggableLayer(layerConfig);
				}
				//layer resize functionality
				//if (isStrechable) {
				//	(<any>$('#' + id)).resizable({
				//		minWidth: $('#' + id).width(),
				//		minHeight: $('#' + id).height(),
				//		alsoResize: '#' + id + '_Body'
				//	});
				//}
				UI.DraggableLayerManager.AddLayerToPage(layer);
				return layer;
			};
			DraggableLayerManager.DraggableLayerHash = new pi.HashTable();
			DraggableLayerManager.DraggableLayerList = [];
			return DraggableLayerManager;
		}());
		UI.DraggableLayerManager = DraggableLayerManager;
		/**
        * Create a dynamic Draggable Layer on the page and return it's ID
        * @param id - optional string identifier of the Draggable layer.  if not specified will begin with "DraggableLayer"
        * @param makeUnique - optional flag indicating to make the id unique using a date timestamp string.  defaults to false unless no id is specified.
        * @param width - optional flag indicating the width of the draggable layer, must be greater than or equal to 450 pixels
        * @param removeOnClose - optional flag indicating to remove this draggable layer and all it's contents upon closing
        * @param onCloseJS - optional javascript to execute when the close function is called.  Must be properly formatted javascript string enclosed in single quotes.
        * @return string - id of the draggable layer that you can use to set the message body or the title
        */
		function CreateDraggableLayer(id, makeUnique, width, removeOnClose, onCloseJS, isStrechable)
		{
			if (id === void 0) { id = null; }
			if (makeUnique === void 0) { makeUnique = false; }
			if (removeOnClose === void 0) { removeOnClose = false; }
			if (isStrechable === void 0) { isStrechable = false; }
			return DraggableLayerManager.CreateDraggableLayer(id, makeUnique, width, removeOnClose, onCloseJS, isStrechable);
		}
		UI.CreateDraggableLayer = CreateDraggableLayer;
		/**
         * Removes a Draggable Layer from the system.
         * @param id
         */
		function RemoveDraggableLayer(id)
		{
			DraggableLayerManager.Remove(id);
		}
		UI.RemoveDraggableLayer = RemoveDraggableLayer;
	})(UI = pi.UI || (pi.UI = {}));
})(pi || (pi = {}));

(function (pi)
{
	/**
     * This separete section of the UI module is for Tool functions utilized for specific features and functionalities commonly found in the PageTools area.
     */
	var UI;
	(function (UI)
	{
		/**
         * Shows a toast
         * @param toastType
         * @param options
         */
		function ShowToast(type, title, message, options)
		{
			if (type == null || type != "")
			{
				type = "info";
			}
			if (pi.JSTools.IsUndefinedNullOrEmpty(title))
			{
				title = "";
			}
			if (pi.JSTools.IsUndefinedNullOrEmpty(message))
			{
				message = "";
			}

		

			var defaultOptions = {
				"closeButton": true,
				"debug": false,
				"newestOnTop": false,
				"progressBar": false,
				"positionClass": "toast-bottom-right",
				"preventDuplicates": false,
				"onclick": function () { InitTimer(); },
				"showDuration": "300",
				"hideDuration": "1000",
				"timeOut": "4000",
				"extendedTimeOut": "1000",
				"showEasing": "swing",
				"hideEasing": "linear",
				"showMethod": "fadeIn",
				"hideMethod": "fadeOut"
			};
			
			//temp... 
			if(options)
			{	
				defaultOptions = options;
			}

			toastr.options = defaultOptions;

			toastr[type](message, title);
		}
		UI.ShowToast = ShowToast;
		/**
         * Shows the spinner on the screen.
         */
		function ShowSpinner(elementId, x, y)
		{
			// Implement functionality to display the waiting spinner
		}
		UI.ShowSpinner = ShowSpinner;
		/**
         * Hide the spinner
         */
		function HideSpinner(elementId)
		{
			// implement functionality to hide the spinner.
		}
		UI.HideSpinner = HideSpinner;
		/**
         * Returns the path name for a specified Icon within the system.
         * @param iconName - string name of the icon to get the path of.
         * @param iconType - type of the icon to retrieve be it.
         * @return  - string path of the Icon you are looking for
         * @todo - iconType currently only supports a theme's main set of icons and does not look for banner or other type based icon sets.
         */
		function GetIconPath(iconName, iconType)
		{
			var path = pi.Root; // + "/theme/i/";
			if (pi.JSTools.IsUndefinedOrNull(pi.ThemeSettings) || pi.JSTools.IsUndefinedOrNull(pi.ThemeSettings.PageIcon))
			{
				path += "/i/i/";
			}
			else
			{
				path += pi.ThemeSettings.PageIcon + "/";
			}
			path += iconName + ".svg";
			return path;
		}
		UI.GetIconPath = GetIconPath;
		/**
         * Show an element at the specified location on the page in X and Y pixel coordinates
         * @param x - the x position to show the element at
         * @param y - the y position to show the element at
         * @param elementId - the element or id of the element you want to show.
         * @param showVeil - optional boolean flag to show the veil behind the element or not, defaults to false.
         * @param effect - effect to play when showing the element, defaults to empty string.
         * @param offsetX - offset the x position by this amount, defaults to 0;
         * @param offsetY - offset the y positon by this amount, defaults to 0
         * @param allowOutsideWindow - allow the item to exist outside the main window screen, defaults to false
         * @param autoscroll - automatically scroll the window when shown, defaults to true.
         */
		function ShowAtPos(x, y, elementId, showVeil, effect, offsetX, offsetY, allowOutsideWindow, autoscroll)
		{
			if (showVeil === void 0) { showVeil = false; }
			if (effect === void 0) { effect = ""; }
			if (offsetX === void 0) { offsetX = 0; }
			if (offsetY === void 0) { offsetY = 0; }
			if (allowOutsideWindow === void 0) { allowOutsideWindow = false; }
			if (autoscroll === void 0) { autoscroll = true; }
			//UITools.ShowAtXY(x, y, elementId, showVeil, offsetX, offsetY, allowOutsideWindow, autoscroll);
			var element = UI.GetElementById(elementId);
			var iframe = null;
			if (!pi.JSTools.IsUndefinedOrNull(element))
			{
				if (element.parentElement.tagName != "BODY" && element.parentElement.tagName != "FORM")
				{
					element.parentNode.removeChild(element);
					document.forms[0].appendChild(element);
				}
				if (!pi.JSTools.IsUndefinedNullOrEmpty(effect))
				{
					$(element).show(effect);
				}
				else
				{
					element.style.display = "block";
					element.style.visibility = "visible";
				}
				x = x + offsetX;
				y = y + offsetY;
				if (!allowOutsideWindow)
				{
					if (autoscroll)
					{
						var sX = UI.GetPageXScrollByForAdjustedElementWithinWindow(x, element);
						var sY = UI.GetPageYScrollByForAdjustedElementWithinWindow(y, element);
						if (sX != 0 || sY != 0)
						{
							window.scrollBy(sX, sY);
						}
					}
				}
				else
				{
					x = UI.GetPageXCoordinateAdjustedElementWithinWindow(x, element);
					y = UI.GetPageYCoordinateAdjustedElementWithinWindow(y, element);
				}
				element.style.left = x + "px";
				element.style.top = y + "px";
				if (!pi.JSTools.IsUndefinedOrNull(showVeil) && showVeil && !pi.JSTools.IsUndefined(metafuseVeil))
				{
					metafuseVeil.ShowVeilUnderElement(element, null);
				}
			}
			else
			{
				// this should probably really be a thrown exception... as it is most likely a developer error.
				console.error("Unable to locate the element specified in UI.ShowAtXY function.");
			}
		}
		UI.ShowAtPos = ShowAtPos;
		/**
         * Show an Alert to display a message to the user.
         * @param alertConfig - configuration of the alert box used to create the properties of the alert
         * @param excludeVeil - does not show the veil if true.
         * @param effect - effect to play when showing the alert.
         */
		function ShowAlert(alertConfig, excludeVeil, effect)
		{
			if (excludeVeil === void 0) { excludeVeil = false; }
			if (effect === void 0) { effect = ""; }
			var alertLayer = new UI.DraggableLayer(alertConfig);
			UI.DraggableLayerManager.AddLayerToPage(alertLayer);
			UITools.ShowAtCenterScreen(alertLayer.Id, !excludeVeil, effect, 0, 0);
			return alertLayer;
		}
		UI.ShowAlert = ShowAlert;
		/**
         * Show an Alert to display a message to the user a a specific location
         * @param alertConfig
         * @param X
         * @param Y
         * @param excludeVeil
         * @param effect
         */
		function ShowAlertAtPos(alertConfig, X, Y, excludeVeil, effect)
		{
			if (excludeVeil === void 0) { excludeVeil = false; }
			if (effect === void 0) { effect = ""; }
			var alertLayer = new UI.DraggableLayer(alertConfig);
			UI.DraggableLayerManager.AddLayerToPage(alertLayer);
			UITools.ShowAtXY(X, Y, alertLayer.Id, !excludeVeil, 0, 0, false, false);
			return alertLayer;
		}
		UI.ShowAlertAtPos = ShowAlertAtPos;
		/**
         * Show and Alert Box to display a message or confirmation to the user.
         * @param message - message to display to the user
         * @param title - title of the alert box
         * @param excludeVeil - does not show the veil is true
         * @param width - width of the alert box
         * @param effect - effect to play when the alert box is shown
         * @param confirmJSCode - String of javascript to execute when the button is clicked, needs to be single quote encased.
         * @param confirmBtnText - String to display for the button to press
         * @param confirmJSCode2 - String of javascript to execute when the second button is clicked, needs to be single quote encased
         * @param confirmBtnText2 - String to display for the second button to press.  If no JS code then this button is not shown
         * @param confirmJSCode3 - String of javascript to execute when the thrird button is clicked, needs to be single quote encased.
         * @param confirmBtnText3 - String to display for the third button to press.  If no JS code then this button is not shown.
         * @param additionalJSOnCancel - Additional JS to execute when a cancel option button is pressed
         * @param disableCancel - boolean flag indicating to not have a cancel button to press
         */
		function ShowAlertBox(message, title, excludeVeil, width, effect, confirmJSCode, confirmBtnText, confirmJSCode2, confirmBtnText2, confirmJSCode3, confirmBtnText3, additionalJSOnCancel, disableCancel)
		{
			if (excludeVeil === void 0) { excludeVeil = false; }
			if (effect === void 0) { effect = ""; }
			if (confirmJSCode === void 0) { confirmJSCode = ""; }
			if (confirmBtnText === void 0) { confirmBtnText = ""; }
			if (confirmJSCode2 === void 0) { confirmJSCode2 = ""; }
			if (confirmBtnText2 === void 0) { confirmBtnText2 = ""; }
			if (confirmJSCode3 === void 0) { confirmJSCode3 = ""; }
			if (confirmBtnText3 === void 0) { confirmBtnText3 = ""; }
			if (additionalJSOnCancel === void 0) { additionalJSOnCancel = ""; }
			if (disableCancel === void 0) { disableCancel = false; }
			var id = UI.DraggableLayerManager.GetLayerName();
			//var hideJS = "UITools.Hide('" + id + "');";
			var hideJS = 'UITools.Hide("' + id + '");';
			// Create a configuration object for the Alert
			var config = {
				Id: id,
				Message: message,
				Title: title,
				Width: width,
				Cancel: null,
				Options: []
			};
			if (pi.JSTools.IsUndefinedNullOrEmpty(confirmBtnText))
			{
				confirmBtnText = pi.Form.GlobalOK;
			}
			if (pi.JSTools.IsUndefinedNullOrEmpty(confirmBtnText2))
			{
				confirmBtnText2 = pi.Form.GlobalOK;
			}
			if (pi.JSTools.IsUndefinedNullOrEmpty(confirmBtnText3))
			{
				confirmBtnText3 = pi.Form.GlobalOK;
			}
			var addCancelBtn = false;
			// Cancel Always Hides the popup, this will append extra commands.
			if (pi.JSTools.IsUndefinedNullOrEmpty(additionalJSOnCancel))
			{
				additionalJSOnCancel = "";
			}
			else
			{
				config.Cancel = { JSClick: additionalJSOnCancel };
				addCancelBtn = true;
			}
			if (!pi.JSTools.IsUndefinedNullOrEmpty(confirmJSCode))
			{
				var btn = {
					Id: "confirm_" + config.Id,
					Text: confirmBtnText,
					JSClick: confirmJSCode + ";" + hideJS,
					JSKeyDown: '',
					OnKeyDown: null,
					CSS: "Button mR10 mL5"
				};
				btn.OnKeyDown = function (e)
				{
					return EKH(e, btn.Id);
				};
				config.Options.push(btn);
				addCancelBtn = true;
			}
			if (!pi.JSTools.IsUndefinedNullOrEmpty(confirmJSCode2))
			{
				var btn = {
					Id: "",
					Text: confirmBtnText2,
					JSClick: confirmJSCode2 + ";" + hideJS,
					JSKeyDown: "",
					OnKeyDown: null,
					CSS: "Button mR10 mL5"
				};
				config.Options.push(btn);
				addCancelBtn = true;
			}
			if (!pi.JSTools.IsUndefinedNullOrEmpty(confirmJSCode3))
			{
				var btn = {
					Id: "",
					Text: confirmBtnText3,
					JSClick: confirmJSCode3 + ";" + hideJS,
					JSKeyDown: "",
					OnKeyDown: null,
					CSS: "Button mR10 mL5"
				};
				config.Options.push(btn);
				addCancelBtn = true;
			}
			if (addCancelBtn && !disableCancel)
			{
				var cancelBtn = {
					Text: pi.Form.GlobalCancel,
					JSClick: hideJS
				};
				config.Options.push(cancelBtn);
			}
			UI.ShowAlert(config, excludeVeil, effect);
			UITools.FocusOnElement('confirm_' + config.Id);
			return UI.GetElementById(config.Id);
		}
		UI.ShowAlertBox = ShowAlertBox;
		/**
         * Hides the element's visibility and display properties
         * @param elementId
         */
		function Hide(elementId, effect)
		{
			// work around for now.
			UITools.Hide(elementId, effect);
		}
		UI.Hide = Hide;
		function HideAndRemove(elementId, effect)
		{
			// not implemented... need to hook into when the effect is finished playing... until then
			UITools.Hide(elementId, effect);
		}
		UI.HideAndRemove = HideAndRemove;
		/**
         * Inserts html to the body of the page.
         * @param html - html to insert.
         */
		function InsertHTML(html)
		{
			if (!pi.JSTools.IsUndefinedNullOrEmpty(html))
			{
				if (document.body && document.body.insertAdjacentElement)
				{
					document.forms[0].insertAdjacentHTML('BeforeEnd', html);
				}
				else if (document.createRange)
				{
					var range = document.createRange();
					if (range.createContextualFragment)
					{
						range.selectNodeContents(document.body);
						var docFrag = range.createContextualFragment(html);
						document.forms[0].appendChild(docFrag);
					}
				}
			}
		}
		UI.InsertHTML = InsertHTML;
		/**
         * Gets the amount of left scroll for the window
         * @return number - left scroll of the window.
         */
		function GetScrollLeft()
		{
			var scrollLeft = document.body.scrollLeft;
			var scrollLeftDocElement = 0;
			if (document.documentElement != null)
			{
				scrollLeftDocElement = document.documentElement.scrollLeft;
			}
			return Math.max(scrollLeft, scrollLeftDocElement);
		}
		UI.GetScrollLeft = GetScrollLeft;
		/**
         * Gets the amount of top scroll for the window
         * @return number - top scroll of the window.
         */
		function GetScrollTop()
		{
			var scrollTop = document.body.scrollTop;
			var scrollTopDocument = 0;
			if (document.documentElement != null)
			{
				scrollTopDocument = document.documentElement.scrollTop;
			}
			return Math.max(scrollTop, scrollTopDocument);
		}
		UI.GetScrollTop = GetScrollTop;
		/**
         * Get the height of the window
         * @return number - height of the window
         */
		function GetWindowHeight()
		{
			var height = document.documentElement.clientHeight;
			if (pi.JSTools.IsUndefinedOrNull(height) || height <= 0)
			{
				height = document.body.clientHeight;
			}
			return height;
		}
		UI.GetWindowHeight = GetWindowHeight;
		/**
         * Gets the width of the window
         * @return number - width of the window.
         */
		function GetWindowWidth()
		{
			var width = document.documentElement.clientWidth;
			if (width == null || width <= 0)
			{
				width = document.body.clientWidth;
			}
			return width;
		}
		UI.GetWindowWidth = GetWindowWidth;
		/**
         * Get the adjusted x coordinate so that the supplied element falls within the windows visible bounds.
         * @param x - number coordinate
         * @param element - html element to determin the adjusted in screen x coordinate
         * @return number - adjusted x coordinate within the window.
         */
		function GetPageXCoordinateAdjustedElementWithinWindow(x, element)
		{
			var scrollLeft = UI.GetScrollLeft();
			var windowWidth = UI.GetWindowWidth();
			var xOver = (x + element.offsetWidth) - (windowWidth + scrollLeft);
			if (xOver > 0)
			{
				x = x - xOver;
			}
			if (x < scrollLeft)
			{
				x = scrollLeft;
			}
			return x;
		}
		UI.GetPageXCoordinateAdjustedElementWithinWindow = GetPageXCoordinateAdjustedElementWithinWindow;
		/**
         * Gets the adjusted y coordinate so that the supplied element falls within the windows Y visible bounds.
         * @param y - y coordinate
         * @param element - html element to determin the adjusted in screen y coordiante
         * @return number - adjusted y coordinate within the window.
         */
		function GetPageYCoordinateAdjustedElementWithinWindow(y, element)
		{
			var scrollTop = UI.GetScrollTop();
			var windowHeight = UI.GetWindowHeight();
			var yOver = (y + element.offsetHeight) - (windowHeight + scrollTop);
			if (yOver > 0)
			{
				y = y - yOver;
			}
			if (y < scrollTop)
			{
				y = scrollTop;
			}
			return y;
		}
		UI.GetPageYCoordinateAdjustedElementWithinWindow = GetPageYCoordinateAdjustedElementWithinWindow;
		/**
         * Get the X amount to scroll the window by so that the supplied element appears within the windows visible bounds
         * @param x - number - x coordinate
         * @param element - html element to determin the adjusted screen x coordinate.
         * @return number - x amount to scroll by to keep the element in the window
         */
		function GetPageXScrollByForAdjustedElementWithinWindow(x, element)
		{
			var scrollLeft = UI.GetScrollLeft();
			var windowWidth = UI.GetWindowWidth();
			var xOver = (x + element.offsetWidth) - (windowWidth - scrollLeft);
			if (xOver > 0 || x < scrollLeft)
			{
				return xOver;
			}
			return 0;
		}
		UI.GetPageXScrollByForAdjustedElementWithinWindow = GetPageXScrollByForAdjustedElementWithinWindow;
		/**
         * Get the Y amount to scroll the window by so that the supplied element appears within the windows visible bounds.
         * Gets the page scroll by y so the window can be adjusted so the element will be within the window for the supplied element.
         * @param y - number y coordinate the element is currently at.
         * @param element - DOM element to determine the adjusted screen y coordinate
         * @return number y amount to scroll by to keep the element in the window
         */
		function GetPageYScrollByForAdjustedElementWithinWindow(y, element)
		{
			var scrollTop = UI.GetScrollTop();
			var windowHeight = UI.GetWindowHeight();
			var yOver = (y + element.offsetHeight) - (windowHeight + scrollTop);
			if (yOver > 0 || y < scrollTop)
			{
				return yOver;
			}
			return 0;
		}
		UI.GetPageYScrollByForAdjustedElementWithinWindow = GetPageYScrollByForAdjustedElementWithinWindow;
	})(UI = pi.UI || (pi.UI = {}));
})(pi || (pi = {}));

(function (pi)
{
	/**
     * This section of the UI module deals exclusively with automating the form system.
     */
	var Rest;
	(function (Rest)
	{
		/**
         * Simple function to access the ClientFactory and retrieve Clients based on input parameters
         * @param clientType - string client type to retrieve
         */
		function GetClient(clientType)
		{
			return Rest.ClientFactory.GetClient(clientType);
		}
		Rest.GetClient = GetClient;
		/**
         * Loads the options of a MetafuseCombobox or a Select Box.
         * @param element	- HtmlElement this function is attached to.
         * @param clientType - client type to retrieve
         * @param operation - client operation to call to execute the operation.
         * @param properties - additional properties for retrival.
         */
		function LoadOptions(element, clientType, operation, properties)
		{
			var property = pi.UI.GetElementProperty(element);
			var scope = pi.UI.GetElementScope(element);
			var client = pi.Rest.GetClient(clientType);
			var elementJson = pi.UI.GetElementJson(element);
			var data = pi.Form.GetInput(property, scope);
			if (pi.JSTools.IsUndefinedNullOrEmpty(elementJson))
			{
				if (!pi.JSTools.IsUndefinedOrNull(client[operation]))
				{
					client[operation](function (response)
					{
						if (response.Success)
						{
							if (data.Type == "select-multiple" || data.Type == "select-one")
							{
								var selectElement = element;
								var options = "<option>Select a Type</option>";
								var projectTypes = response.Json;
								$(element).attr("data-pi-json", JSON.stringify(projectTypes));
								for (var i = 0; i < projectTypes.length; ++i)
								{
									options += "<option value='" + projectTypes[i].Id + "' data-pi-json='" + JSON.stringify(projectTypes[i]) + "' >" + projectTypes[i].Name + "</option>";
								}
								$(element).html(options);
							}
							else
							{
								// fix this.
								alert("Options other than basic selection is not implemented");
							}
						}
					});
				}
			}
		}
		Rest.LoadOptions = LoadOptions;
		/**
         * Saves the object defined by the form.
         * @param element - Save element of the form.
         */
		function Save(element)
		{
			var scope = pi.UI.GetElementScope(element);
			var $parentScope = pi.UI.GetElementParentScope(element);
			var Id = $parentScope.attr("data-pi-id");
			var clientType = $parentScope.attr("data-pi-client");
			var piObject = pi.Form.GetObject(scope);
			if (!pi.JSTools.IsUndefinedOrNull(Id))
			{
				piObject["Id"] = Id;
			}
			var client = pi.Rest.GetClient(clientType);
			if (client != null)
			{
				client.Save(piObject, pi.Rest.DefaultResponseHandler);
			}
		}
		Rest.Save = Save;
		/**
         * Delete an object defined in the parent scope id.
         * @param element - element making the delete call.
         */
		function Delete(element)
		{
			var $parentScope = pi.UI.GetElementParentScope(element);
			var Id = $parentScope.attr("data-pi-id");
			var clientType = $parentScope.attr("data-pi-client");
			var client = pi.Rest.GetClient(clientType);
			if (!pi.JSTools.IsUndefinedOrNull(Id) && client != null)
			{
				client.Delete(Id, pi.Rest.DefaultResponseHandler);
			}
		}
		Rest.Delete = Delete;
		/**
         * When a selection is made, we can use this to set the properties of a "target" stop form.
         * @param element - Element making the selection choise
         * @param targetScope - Target scope to load
         */
		function SetSelectedProperties(element, targetScope)
		{
			var property = pi.UI.GetElementProperty(element);
			var $scope = pi.UI.GetElementParentScope(element);
			var scope = null;
			if ($scope.length > 0)
			{
				scope = $scope.attr("data-pi-scope");
			}
			var selectedData = pi.Form.GetSelectedJson(property, scope);
			if (selectedData != null)
			{
				var selectedItem = null;
				if (pi.JSTools.IsUndefined(selectedData.length))
				{
					selectedItem = selectedData;
				}
				else
				{
					selectedItem = selectedData[0];
				}
				for (var propKey in selectedItem)
				{
					pi.Form.Set(selectedItem[propKey], propKey, targetScope);
				}
				var $scopeTarget = pi.UI.GetScopeJQuery(targetScope);
				$scopeTarget.attr("data-pi-id", selectedItem.Id);
			}
		}
		Rest.SetSelectedProperties = SetSelectedProperties;
		/**
         * Default Response Handler for the Automated Form.
         * @param response
         */
		function DefaultResponseHandler(response)
		{
			if (response.Success)
			{
				pi.UI.ShowAlertBox("Operation Success", "Success");
				pi.Rest.ResetRestForm();
			}
			else
			{
				pi.UI.ShowAlertBox("Operation Failed", "Success");
			}
		}
		Rest.DefaultResponseHandler = DefaultResponseHandler;
		/**
         * Resets the Automated Form when a Rest Response action is completed.
         */
		function ResetRestForm()
		{
			$("[data-pi-json]").each(function (index, element)
			{
				$(element).attr("data-pi-json", "");
			});
			$("[data-pi-id]").each(function (index, element)
			{
				$(element).attr("data-pi-id", "");
			});
		}
		Rest.ResetRestForm = ResetRestForm;
		/**
         * Onload function fired when the page loads to wireup the form.
         */
		function AutomateForm()
		{
			$("[data-pi-load]").each(function (index, e)
			{
				var $parentScope = pi.UI.GetElementParentScope(e);
				var scope = $parentScope.attr("data-pi-scope");
				var property = pi.UI.GetElementProperty(e);
				var operation = pi.UI.GetElementLoadValue(e);
				var clientType = pi.UI.GetElementClientValue(e);
				var data = pi.Form.GetInput(property, scope);
				if (data.Type == "select-one" || data.Type == "select-multiple")
				{
					pi.Rest.LoadOptions(e, clientType, operation, "");
					$(e).mouseover(function (ev)
					{
						pi.Rest.LoadOptions(e, clientType, operation, "");
					});
					var targetScope = pi.UI.GetElementTargetScope(e);
					if (!pi.JSTools.IsUndefinedNullOrEmpty(targetScope))
					{
						$(e).change(function (ev)
						{
							pi.Rest.SetSelectedProperties(e, targetScope);
						});
					}
				}
			});
			$("[data-pi-action]").each(function (index, e)
			{
				if ($(e).attr("data-pi-action") == "Save")
				{
					$(e).click(function (ev)
					{
						pi.Rest.Save(e);
					});
				}
				else if ($(e).attr("data-pi-action") == "Delete")
				{
					$(e).click(function (ev)
					{
						pi.Rest.Delete(this);
					});
				}
			});
		}
		Rest.AutomateForm = AutomateForm;
	})(Rest = pi.Rest || (pi.Rest = {}));
})(pi || (pi = {}));
// jQuery document ready function hookup.
$(function ()
{
	pi.Rest.AutomateForm();
});

(function (pi)
{
	/**
     * This separete section of the UI module is for Local Storage functions utilized for specific features and functionalities commonly found in the PageTools area.
     */
	var UI;
	(function (UI)
	{
		var LocalStorage = (function ()
		{
			function LocalStorage()
			{
			}
			/**
             * Sets the local storage key-value pair along with its associated expiration.
             * @param name - name/key of local storage
             * @param value - value of local storage, preferably stringified JSON
             * @param expires - expiration of local storage.  No storage if null or undefined.
             */
			LocalStorage.Set = function (name, value, expires)
			{
				if (name.indexOf(pi.Root) < 0)
				{
					name = pi.Root + name;
				}

				if (pi.JSTools.IsUndefinedOrNull(expires) || expires > 0)
				{
					if (pi.JSTools.IsUndefinedOrNull(expires))
					{
						expires = 7200; // default 2h
					}
						
					var date = new Date();
					var schedule = Math.round((date.setSeconds(date.getSeconds() + expires)) / 1000);
					localStorage.setItem(name, value);
					localStorage.setItem(name + '_time', schedule.toString());
				}
				else if (expires === 0)
				{
					// don't set in local storages
				}
				
			};
			/**
             * Gets the local storage value from the key supplied. If value is expired or does not exist yet, function gets the value thru REST and sets it to local storage.
             * @param name - key in the local storage
             */
			LocalStorage.Get = function (name, done, modelProperties)
			{
				if (name.indexOf(pi.Root) < 0)
				{
					name = pi.Root + name;
				}
				var info;
				if (localStorage.getItem(name) != null)
				{
					if (this.CheckExpired(name) || localStorage.getItem(name) == "null")
					{
						this.Remove(name);
						info = "";
					}
					else
					{
						info = JSON.parse(localStorage.getItem(name));
					}
				}
				else
				{
					info = "";
				}
				return info;
			};
			/**
             * Removes the local storage key-value pair along with its associated expiration
             * @param name - name of local storage
             */
			LocalStorage.Remove = function (name)
			{
				if (name.indexOf(pi.Root) < 0)
				{
					name = pi.Root + name;
				}
				localStorage.removeItem(name);
				localStorage.removeItem(name + '_time');
			};
			/**
             * Clears the entire local storage cache
             */
			LocalStorage.RemoveAll = function ()
			{
				localStorage.clear();
			};
			/**
             * Gets the time that the supplied local storage key expires at
             * @param key
             */
			LocalStorage.GetExpirationTime = function (key)
			{
				var storedTime = localStorage.getItem(name + '_time');
				if (storedTime == undefined || storedTime == 'null')
				{
					storedTime = -1;
				}
				return storedTime;
			};
			/**
             * Checks whether the supplied local storage is expired
             * @param name - name of local storage
             * @returns - true if expired, false if not expired
             */
			LocalStorage.CheckExpired = function (name)
			{
				if (name.indexOf(pi.Root) < 0)
				{
					name = pi.Root + name;
				}
				var date = Date.now();
				var current = Math.round(+date / 1000);
				var storedTime = localStorage.getItem(name + '_time');
				if (storedTime == undefined || storedTime == 'null')
				{
					storedTime = 0;
				}
				if (storedTime < current)
				{
					this.Remove(name);
					return true;
				}
				return false;
			};
			/**
             * Changes local storage key, maintains value and expiration time
             * @param oldKey - the old name/key identifier
             * @param newKey - the new name/key identifier
             */
			LocalStorage.ChangeKey = function (oldKey, newKey)
			{
				if (oldKey != "" && newKey != "")
				{
					var value = localStorage.getItem(oldKey);
					var expirTime = this.GetExpirationTime(oldKey);
					this.Remove(oldKey);
					this.Set(newKey, value, expirTime);
				}
			};
			return LocalStorage;
		}());
		UI.LocalStorage = LocalStorage;
	})(UI = pi.UI || (pi.UI = {}));
})(pi || (pi = {}));

(function (pi)
{
	/**
     * UI Module for modifying and altering the UI and DOM.  Functions that are suffixed with JQuery return a JQuery array of elements, functions that have no suffix return a specific Element.
     * This section/file is dedicated to the core functionalities found within the pi.UI module that are essential to retrieving DOM elements.
     */
	var UI;
	(function (UI)
	{
		/**
         * Reference to object Icons
         */
		UI.Img = null;
		/**
         * This function works partly as a safty check as well as a utility to retrieve an element by it's specified ID.
         * @param elementId - this can be an DOM element or a string indicating the ID of an element to retrieve.  If it is an element it is returned but if it is a string then we retrieve the element from the DOM and return it
         * @return HTMLElement - the DOM element with the ID we are looking for.
         */
		function GetElementById(elementId)
		{
			if (pi.JSTools.IsString(elementId))
			{
				if (pi.JSTools.IsUndefinedNullOrEmpty(elementId))
				{
					return null;
				}
				else
				{
					var element = document.getElementById(elementId);
					return element;
				}
			}
			else
			{
				return elementId;
			}
		}
		UI.GetElementById = GetElementById;
		/**
         * Retrieve an array of DOM elements using JQuery that contains a specific data dash attribute. This list can be filtered down to specific value, scope, and element types.
         * @param dataDash - the string data dash property we are looking for.  You do not have to supply the "data-" part of the attribute but will still work if you do.
         * @param value - optional value of the data- attribute you are looking for.  If none is specified then all elements with the data- property will be returned.
         * @param scope - optional scope defines a top level element with a data-pi-scope attribute to search within.  If none is specified then all elements in the "body" tag will be returned
         * @param elementType - optional filter to specify the exact element type to use such as "div", "input", etc.
         * @return JQuery - a query array of all elements that have that data- attribute with filtered options.
         */
		function GetDataDashJQuery(dataDash, value, scope, elementType)
		{
			var dataDashSearch = "";
			var ddIdx = dataDash.indexOf("data-"); // get the first index of data-
			//if the data- index is not present or "data-" is not the first part of the property add data- to the front.
			// This could be something like data-my-data-attr in which data- is legitimately part of the attribute but not at the front.
			if (ddIdx != 0)
			{
				dataDash += "data-" + dataDash;
			}
			// If a value is supplied we are looking for a specific data- element with a specific value.  Otherwise we are just looking for all data- elements
			if (!pi.JSTools.IsUndefinedNullOrEmpty(value))
			{
				dataDashSearch += "[" + dataDash + "=" + value + "]";
			}
			else
			{
				dataDashSearch += "[" + dataDash + "]";
			}
			// If we are looking for a specfic element type, most commonly an input type
			if (!pi.JSTools.IsUndefinedNullOrEmpty(elementType))
			{
				dataDashSearch += elementType + dataDashSearch;
			}
			// if we are refining to all elements within a specific scope.
			if (!pi.JSTools.IsUndefinedNullOrEmpty(scope))
			{
				dataDashSearch = "[data-pi-scope=" + scope + "] " + dataDashSearch;
			}
			return $(dataDashSearch);
		}
		UI.GetDataDashJQuery = GetDataDashJQuery;
		/**
         * Get an array of all elements that have the specified data-pi-scope attribute value.
         * @param scope - scope value to search for.
         * @return JQuery - array of all elements with the scope attribute.
         */
		function GetScopeJQuery(scope)
		{
			var scopedElement = null;
			if (pi.JSTools.IsUndefinedNullOrEmpty(scope))
			{
				scopedElement = $(pi.DefaultScope);
			}
			else
			{
				scopedElement = $("[data-pi-scope=" + scope + "]");
			}
			return scopedElement;
		}
		UI.GetScopeJQuery = GetScopeJQuery;
		/**
         * Get a single element that defines the specified scope.
         * @param scope - scope value to search for
         * @param index - optional value if you are sure you have more than a single element with the specified scope.  defaults to 0.
         * @return HTMLElement - element retrieved from the search.
         */
		function GetScope(scope, index)
		{
			if (index === void 0) { index = 0; }
			var $scope = UI.GetScopeJQuery(scope);
			if ($scope.length > index)
			{
				return $scope.get()[index];
			}
			return null;
		}
		UI.GetScope = GetScope;
		/**
         * Get an Element by specified property and scope
         * @param property - property value to search for.
         * @param scope - scope value to search for.
         * @param elementType - optional parameter to specify the type of element to retrieve.
         * @param index - optional index if multiple properties exist.  defaults to 0.
         * @return HTMLElement - element retrieved from the search.  If nothing was found then return is null.
         */
		function GetElement(property, scope, elementType, index)
		{
			if (index === void 0) { index = 0; }
			var $elements = UI.GetJQuery(property, scope, elementType);
			if ($elements.length > index)
			{
				return $elements.get()[index];
			}
			return null;
		}
		UI.GetElement = GetElement;
		/**
         * Gets the jQuery array of Elements by specified property and scope
         * @param property - property value to look for
         * @param scope - optional scope value to search by
         * @param elementType - optional element type to filter by
         * @return JQuery - jQuery array of objects to return.
         */
		function GetJQuery(property, scope, elementType)
		{
			return UI.GetDataDashJQuery("data-pi-property", property, scope, elementType);
		}
		UI.GetJQuery = GetJQuery;
		/**
         * Get all property values found within the specified scope value.
         * @param scope - scope value to get properties for
         * @param excludeSubScopes - optional value that will exclude any property that defines a new scope.  defaults to true
         * @return jQuery - array of elements to return
         */
		function GetPropertiesInScope(scope, excludeSubScopes)
		{
			if (excludeSubScopes === void 0) { excludeSubScopes = true; }
			var $properties = UI.GetDataDashJQuery("data-pi-property", null, scope, null);
			if (excludeSubScopes)
			{
				$properties = $properties.not($properties.filter("[data-pi-scope]").find("[data-pi-property]"));
			}
			return $properties;
		}
		UI.GetPropertiesInScope = GetPropertiesInScope;
		/**
         * Get property supplied to this element.
         * @param element - element to find the property of.
         */
		function GetElementProperty(element)
		{
			return $(element).attr("data-pi-property");
		}
		UI.GetElementProperty = GetElementProperty;
		/**
         * Get the Scope of the supplied element.
         * @param element - element to find the scope of.
         */
		function GetElementScope(element)
		{
			return $(element).attr("data-pi-scope");
		}
		UI.GetElementScope = GetElementScope;
		/**
         * Retrieve any json information contained on the element.
         * @param element - element to get the json from.
         */
		function GetElementJson(element)
		{
			return $(element).attr("data-pi-json");
		}
		UI.GetElementJson = GetElementJson;
		/**
         * For certain controls they need a special selected data dash property.
         * @param element - element to get the selected json from.
         */
		function GetSelectedJson(element)
		{
			return $(element).attr("data-pi-selected-json");
		}
		UI.GetSelectedJson = GetSelectedJson;
		/**
         * Traverse Up the chain of parent elements until we find a scope object.
         * @param element
         */
		function GetElementParentScope(element)
		{
			return $(element).parents("[data-pi-scope]");
		}
		UI.GetElementParentScope = GetElementParentScope;
		/**
         * Get the pi identifier of the object.
         * @param element - element id.
         */
		function GetElementPIId(element)
		{
			return $(element).attr("data-pi-id");
		}
		UI.GetElementPIId = GetElementPIId;
		/**
         * Get the value of the Load option of the element.
         * @param element - element to retreive the value from.
         */
		function GetElementLoadValue(element)
		{
			return $(element).attr("data-pi-load");
		}
		UI.GetElementLoadValue = GetElementLoadValue;
		/**
         * Get the client property of the element.
         * @param element - element to retreive the value from.
         */
		function GetElementClientValue(element)
		{
			return $(element).attr("data-pi-client");
		}
		UI.GetElementClientValue = GetElementClientValue;
		/**
         * Get the target scope of of this input
         * @param element - element to retreive the value from.
         */
		function GetElementTargetScope(element)
		{
			return $(element).attr("data-pi-target");
		}
		UI.GetElementTargetScope = GetElementTargetScope;
	})(UI = pi.UI || (pi.UI = {}));
})(pi || (pi = {}));

var PI = pi;
