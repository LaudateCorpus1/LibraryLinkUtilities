(* Wolfram Language Test file *)
TestRequirement[$VersionNumber > 12.0]
TestExecute[
	currentDirectory = DirectoryName[$CurrentFile];
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];
	sourceDirectory = FileNameJoin[{currentDirectory, "TestSources"}];
	Get[FileNameJoin[{sourceDirectory, "NumericArrayOperations.wl"}]];
	
	na = NumericArray[{1, 2, 3, 4}];
]

(****************************NumericArray Operations****************************************)
Test[
	emptyVector[]
	,
	{}
	,
	TestID->"NumericArrayTestSuite-20190726-G2E3E0"
];

Test[
	Dimensions @ emptyMatrix[]
	,
	{3, 5, 0}
	,
	TestID->"NumericArrayTestSuite-20190726-M2J7A2"
];

Test[
	testDimensions[{}]
	,
	Failure["DimensionsError", <|
		"MessageTemplate" -> "An error caused by inconsistent dimensions or by exceeding array bounds.", 
		"MessageParameters" -> <||>, 
		"ErrorCode" -> 3, 
		"Parameters" -> {}|>
	]
	,
	TestID->"NumericArrayTestSuite-20190729-X1X5Q8"
];

Test[
	Normal @* testDimensions /@ {{0}, {3}, {3, 0}, {3, 2}, {3, 2, 0}, {3, 2, 4}}
	,
	{
		{}, 
		{0., 0., 0.}, 
		{{}, {}, {}}, 
		{{0., 0.}, {0., 0.}, {0., 0.}}, 
		{{{}, {}}, {{}, {}}, {{}, {}}}, 
		{{{0., 0., 0., 0.}, {0., 0., 0., 0.}}, {{0., 0., 0., 0.}, {0., 0., 0., 0.}}, {{0., 0., 0., 0.}, {0., 0., 0., 0.}}}
	}
	,
	TestID->"NumericArrayTestSuite-20190729-R3O9K3"
];

Test[
	Normal /@ List @@ testDimensions2[]
	,
	{
		{}, 
		{0., 0., 0.}, 
		{{}, {}, {}}, 
		{{0., 0.}, {0., 0.}, {0., 0.}}, 
		{{{}, {}}, {{}, {}}, {{}, {}}}, 
		{{{0., 0., 0., 0.}, {0., 0., 0., 0.}}, {{0., 0., 0., 0.}, {0., 0., 0., 0.}}, {{0., 0., 0., 0.}, {0., 0., 0., 0.}}}
	}
	,
	TestID->"NumericArrayTestSuite-20190729-I2O3D2"
];

Test[
	echoNumericArray[na]
	,
	NumericArray[{1, 2, 3, 4}, "UnsignedInteger8"]
	,
	TestID->"NumericArrayOperations-20150825-M7G1B2"
]

Test[
	num = NumericArray[N @ Range[0, 47]/47, "Real64"];
	res = NumericArrayQ @ echoNumericArray[num]
	,
	True
	,
	TestID->"NumericArrayOperations-20150825-P4U4W5"
]

ExactTest[
	getNALength[na]
	,
	4
	,
	TestID->"NumericArrayOperations-20150827-V6M5C7"
]

ExactTest[
	getNARank[na]
	,
	1
	,
	TestID->"NumericArrayOperations-20150827-A7Q6J1"
]

Test[
	NumericArrayQ @ newNA[]
	,
	True
	,
	TestID->"NumericArrayOperations-20150827-B3Y1C2"
]

Test[
	cloneNA[na]
	,
	na
	,
	TestID->"NumericArrayOperations-20150827-I0C3X0"
]

Test[
	cloneNA[NumericArray[{}, "UnsignedInteger8"]]
	,
	NumericArray[{}, "UnsignedInteger8"]
	,
	TestID -> "NumericArrayTestSuite-20190729-V6U8K6"
]

Test[(*check NumericArray shared APi's*)
	changeSharedNA[na];
	sna = getSharedNA[];
	SameQ[sna, na]
	,
	True
	,
	TestID->"NumericArrayOperations-20150827-O5S8C2"
]

Test[
	accumulateIntegers[NumericArray[Range[10]]]
	,
	Total @ Range[10]
	,
	TestID->"NumericArrayTestSuite-20181030-U5E5U6"
]

Test[
	accumulateIntegers[NumericArray[{3.5}]]
	,
	Failure["FunctionError", <|"MessageTemplate" -> "An error occurred in the library function.", "MessageParameters" -> <||>, "ErrorCode" -> 6, "Parameters" -> {}|>]
	,
	TestID->"NumericArrayTestSuite-20181030-P4G8W4"
]

Test[
	convertMethodName /@ Range[8]
	,
	{"Check", "ClipCheck", "Coerce", "ClipCoerce", "Round", "ClipRound", "Scale", "ClipScale"}
	,
	TestID->"NumericArrayTestSuite-20190328-S2I8Q2"
]

TestMatch[
	convertMethodName[9]
	,
	_Failure
	,
	TestID->"NumericArrayTestSuite-20190328-R8Y1F1"
]

Test[
	convert[NumericArray[{3.5}], 5 (* Round *), 0]
	,
	NumericArray[NumericArray[{3.5}], "UnsignedInteger16", "Round"]
	,
	TestID->"NumericArrayTestSuite-20181105-I0C6A3"
]

TestMatch[
	convert[NumericArray[{3.5}], 1 (* Check *), 0]
	,
	Failure["NumericArrayConversionError", <|
		"MessageTemplate" -> "Failed to convert NumericArray from different type.", 
		"MessageParameters" -> <||>, 
		"ErrorCode" -> _?CppErrorCodeQ, 
		"Parameters" -> {}|>
	]
	,
	TestID->"NumericArrayTestSuite-20181105-P7M0S7"
]

Test[
	convert[NumericArray[Range[10]], 8 (* ClipAndScale *), 1]
	,
	NumericArray[NumericArray[Range[10]], "UnsignedInteger16", "ClipAndScale"]
	,
	TestID->"NumericArrayTestSuite-20181105-W8Z5G6"
]


EndRequirement[]