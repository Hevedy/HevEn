/* Copyright (c) <2003-2016> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include "dContainersStdAfx.h"
#include "dCRC.h"

/*
static dCRCTYPE randBits0[] =
{
	7266447313870364031ULL,  4946485549665804864ULL, 16945909448695747420ULL, 16394063075524226720ULL,  
	4873882236456199058ULL, 14877448043947020171ULL,  6740343660852211943ULL, 13857871200353263164ULL,
	5249110015610582907ULL, 10205081126064480383ULL,  1235879089597390050ULL, 17320312680810499042ULL,
	16489141110565194782ULL,  8942268601720066061ULL, 13520575722002588570ULL, 14226945236717732373ULL, 

	9383926873555417063ULL, 15690281668532552105ULL, 11510704754157191257ULL, 15864264574919463609ULL, 
	6489677788245343319ULL,  5112602299894754389ULL, 10828930062652518694ULL, 15942305434158995996ULL,
	15445717675088218264ULL, 4764500002345775851ULL, 14673753115101942098ULL,  236502320419669032ULL, 
	13670483975188204088ULL, 14931360615268175698ULL, 8904234204977263924ULL, 12836915408046564963ULL, 

	12120302420213647524ULL, 15755110976537356441ULL,  5405758943702519480ULL, 10951858968426898805ULL, 
	17251681303478610375ULL,  4144140664012008120ULL, 18286145806977825275ULL, 13075804672185204371ULL, 
	10831805955733617705ULL,  6172975950399619139ULL, 12837097014497293886ULL, 12903857913610213846ULL,
	560691676108914154ULL,    1074659097419704618ULL, 14266121283820281686ULL, 11696403736022963346ULL, 

	13383246710985227247ULL,  7132746073714321322ULL, 10608108217231874211ULL, 9027884570906061560ULL, 
	12893913769120703138ULL, 15675160838921962454ULL,  2511068401785704737ULL, 14483183001716371453ULL, 
	3774730664208216065ULL,  5083371700846102796ULL,  9583498264570933637ULL, 17119870085051257224ULL, 
	5217910858257235075ULL, 10612176809475689857ULL,  1924700483125896976ULL,  7171619684536160599ULL,


	10949279256701751503ULL, 15596196964072664893ULL, 14097948002655599357ULL, 615821766635933047ULL, 
	5636498760852923045ULL, 17618792803942051220ULL, 580805356741162327ULL,   425267967796817241ULL, 
	8381470634608387938ULL, 13212228678420887626ULL, 16993060308636741960ULL, 957923366004347591ULL, 
	6210242862396777185ULL,  1012818702180800310ULL, 15299383925974515757ULL, 17501832009465945633ULL,

	17453794942891241229ULL, 15807805462076484491ULL,  8407189590930420827ULL,   974125122787311712ULL,
	1861591264068118966ULL, 997568339582634050ULL, 18046771844467391493ULL, 17981867688435687790ULL, 
	3809841506498447207ULL,  9460108917638135678ULL, 16172980638639374310ULL,   958022432077424298ULL, 
	4393365126459778813ULL, 13408683141069553686ULL, 13900005529547645957ULL, 15773550354402817866ULL,

	16475327524349230602ULL,  6260298154874769264ULL, 12224576659776460914ULL,  6405294864092763507ULL, 
	7585484664713203306ULL,  5187641382818981381ULL, 12435998400285353380ULL, 13554353441017344755ULL,
	646091557254529188ULL, 11393747116974949255ULL, 16797249248413342857ULL, 15713519023537495495ULL,
	12823504709579858843ULL,  4738086532119935073ULL, 4429068783387643752ULL,  585582692562183870ULL,

	1048280754023674130ULL, 6788940719869959076ULL, 11670856244972073775ULL, 2488756775360218862ULL,
	2061695363573180185ULL,  6884655301895085032ULL, 3566345954323888697ULL, 12784319933059041817ULL,
	4772468691551857254ULL,  6864898938209826895ULL,  7198730565322227090ULL, 2452224231472687253ULL,
	13424792606032445807ULL, 10827695224855383989ULL, 11016608897122070904ULL, 14683280565151378358ULL,

	7077866519618824360ULL, 17487079941198422333ULL, 3956319990205097495ULL,  5804870313319323478ULL,
	8017203611194497730ULL,  3310931575584983808ULL,  5009341981771541845ULL, 11772020174577005930ULL,
	3537640779967351792ULL,  6801855569284252424ULL, 17687268231192623388ULL, 12968358613633237218ULL, 
	1429775571144180123ULL, 10427377732172208413ULL, 12155566091986788996ULL, 16465954421598296115ULL, 

	12710429690464359999ULL, 9547226351541565595ULL, 12156624891403410342ULL,  2985938688676214686ULL,
	18066917785985010959ULL,  5975570403614438776ULL, 11541343163022500560ULL, 11115388652389704592ULL,
	9499328389494710074ULL,  9247163036769651820ULL,  3688303938005101774ULL, 2210483654336887556ULL,
	15458161910089693228ULL,  6558785204455557683ULL,  1288373156735958118ULL, 18433986059948829624ULL,

	3435082195390932486ULL, 16822351800343061990ULL,  3120532877336962310ULL, 16681785111062885568ULL,  
	7835551710041302304ULL, 2612798015018627203ULL, 15083279177152657491ULL, 6591467229462292195ULL,
	10592706450534565444ULL,  7438147750787157163ULL, 323186165595851698ULL, 7444710627467609883ULL,
	8473714411329896576ULL,  2782675857700189492ULL,  3383567662400128329ULL, 3200233909833521327ULL,

	12897601280285604448ULL, 3612068790453735040ULL, 8324209243736219497ULL, 15789570356497723463ULL,
	1083312926512215996ULL,  4797349136059339390ULL, 5556729349871544986ULL, 18266943104929747076ULL,
	1620389818516182276ULL, 172225355691600141ULL,  3034352936522087096ULL,  1266779576738385285ULL,
	3906668377244742888ULL,  6961783143042492788ULL, 17159706887321247572ULL,  4676208075243319061ULL,

	10315634697142985816ULL, 13435140047933251189ULL, 716076639492622016ULL,	13847954035438697558ULL,
	7195811275139178570ULL, 10815312636510328870ULL,  6214164734784158515ULL, 16412194511839921544ULL, 
	3862249798930641332ULL,  1005482699535576005ULL,  4644542796609371301ULL, 17600091057367987283ULL, 
	4209958422564632034ULL, 5419285945389823940ULL, 11453701547564354601ULL,  9951588026679380114ULL,

	7425168333159839689ULL,  8436306210125134906ULL, 11216615872596820107ULL,  3681345096403933680ULL,
	5770016989916553752ULL, 11102855936150871733ULL, 11187980892339693935ULL, 396336430216428875ULL, 
	6384853777489155236ULL,  7551613839184151117ULL, 16527062023276943109ULL, 13429850429024956898ULL,
	9901753960477271766ULL,  9731501992702612259ULL,  5217575797614661659ULL, 10311708346636548706ULL,

	15111747519735330483ULL, 4353415295139137513ULL,  1845293119018433391ULL, 11952006873430493561ULL,
	3531972641585683893ULL, 16852246477648409827ULL,	15956854822143321380ULL, 12314609993579474774ULL,
	16763911684844598963ULL, 16392145690385382634ULL,  1545507136970403756ULL, 17771199061862790062ULL,
	12121348462972638971ULL, 12613068545148305776ULL,   954203144844315208ULL,  1257976447679270605ULL, 

	3664184785462160180ULL,  2747964788443845091ULL, 15895917007470512307ULL, 15552935765724302120ULL,
	16366915862261682626ULL, 8385468783684865323ULL, 10745343827145102946ULL, 2485742734157099909ULL, 
	916246281077683950ULL,	 15214206653637466707ULL, 12895483149474345798ULL,  1079510114301747843ULL,
	10718876134480663664ULL,  1259990987526807294ULL,  8326303777037206221ULL, 14104661172014248293ULL,
};
*/

static dCRCTYPE randBits0[] =
{
    static_cast<long long>(7266447313870364031ULL), static_cast<long long>(4946485549665804864ULL), static_cast<long long>(16945909448695747420ULL), static_cast<long long>(16394063075524226720ULL),
    static_cast<long long>(4873882236456199058ULL), static_cast<long long>(14877448043947020171ULL), static_cast<long long>(6740343660852211943ULL), static_cast<long long>(13857871200353263164ULL),
    static_cast<long long>(5249110015610582907ULL), static_cast<long long>(10205081126064480383ULL), static_cast<long long>(1235879089597390050ULL), static_cast<long long>(17320312680810499042ULL),
    static_cast<long long>(16489141110565194782ULL), static_cast<long long>(8942268601720066061ULL), static_cast<long long>(13520575722002588570ULL), static_cast<long long>(14226945236717732373ULL),

    static_cast<long long>(9383926873555417063ULL), static_cast<long long>(15690281668532552105ULL), static_cast<long long>(11510704754157191257ULL), static_cast<long long>(15864264574919463609ULL), 
    static_cast<long long>(6489677788245343319ULL), static_cast<long long>(5112602299894754389ULL), static_cast<long long>(10828930062652518694ULL), static_cast<long long>(15942305434158995996ULL),
    static_cast<long long>(15445717675088218264ULL), static_cast<long long>(4764500002345775851ULL), static_cast<long long>(14673753115101942098ULL), static_cast<long long>(236502320419669032ULL),
    static_cast<long long>(13670483975188204088ULL), static_cast<long long>(14931360615268175698ULL), static_cast<long long>(8904234204977263924ULL), static_cast<long long>(12836915408046564963ULL),

    static_cast<long long>(12120302420213647524ULL), static_cast<long long>(15755110976537356441ULL), static_cast<long long>(5405758943702519480ULL), static_cast<long long>(10951858968426898805ULL),
    static_cast<long long>(17251681303478610375ULL), static_cast<long long>(4144140664012008120ULL), static_cast<long long>(18286145806977825275ULL), static_cast<long long>(13075804672185204371ULL),
    static_cast<long long>(10831805955733617705ULL), static_cast<long long>(6172975950399619139ULL), static_cast<long long>(12837097014497293886ULL), static_cast<long long>(12903857913610213846ULL),
    static_cast<long long>(560691676108914154ULL), static_cast<long long>(1074659097419704618ULL), static_cast<long long>(14266121283820281686ULL), static_cast<long long>(11696403736022963346ULL),

    static_cast<long long>(13383246710985227247ULL), static_cast<long long>(7132746073714321322ULL), static_cast<long long>(10608108217231874211ULL), static_cast<long long>(9027884570906061560ULL),
    static_cast<long long>(12893913769120703138ULL), static_cast<long long>(15675160838921962454ULL), static_cast<long long>(2511068401785704737ULL), static_cast<long long>(14483183001716371453ULL),
    static_cast<long long>(3774730664208216065ULL), static_cast<long long>(5083371700846102796ULL), static_cast<long long>(9583498264570933637ULL), static_cast<long long>(17119870085051257224ULL),
    static_cast<long long>(5217910858257235075ULL), static_cast<long long>(10612176809475689857ULL), static_cast<long long>(1924700483125896976ULL), static_cast<long long>(7171619684536160599ULL),


    static_cast<long long>(10949279256701751503ULL), static_cast<long long>(15596196964072664893ULL), static_cast<long long>(14097948002655599357ULL), static_cast<long long>(615821766635933047ULL),
    static_cast<long long>(5636498760852923045ULL), static_cast<long long>(17618792803942051220ULL), static_cast<long long>(580805356741162327ULL), static_cast<long long>(425267967796817241ULL),
    static_cast<long long>(8381470634608387938ULL), static_cast<long long>(13212228678420887626ULL), static_cast<long long>(16993060308636741960ULL), static_cast<long long>(957923366004347591ULL),
    static_cast<long long>(6210242862396777185ULL), static_cast<long long>(1012818702180800310ULL), static_cast<long long>(15299383925974515757ULL), static_cast<long long>(17501832009465945633ULL),

    static_cast<long long>(17453794942891241229ULL), static_cast<long long>(15807805462076484491ULL), static_cast<long long>(8407189590930420827ULL), static_cast<long long>(974125122787311712ULL),
    static_cast<long long>(1861591264068118966ULL), static_cast<long long>(997568339582634050ULL), static_cast<long long>(18046771844467391493ULL), static_cast<long long>(17981867688435687790ULL),
    static_cast<long long>(3809841506498447207ULL), static_cast<long long>(9460108917638135678ULL), static_cast<long long>(16172980638639374310ULL), static_cast<long long>(958022432077424298ULL),
    static_cast<long long>(4393365126459778813ULL), static_cast<long long>(13408683141069553686ULL), static_cast<long long>(13900005529547645957ULL), static_cast<long long>(15773550354402817866ULL),

    static_cast<long long>(16475327524349230602ULL), static_cast<long long>(6260298154874769264ULL), static_cast<long long>(12224576659776460914ULL), static_cast<long long>(6405294864092763507ULL),
    static_cast<long long>(7585484664713203306ULL), static_cast<long long>(5187641382818981381ULL), static_cast<long long>(12435998400285353380ULL), static_cast<long long>(13554353441017344755ULL),
    static_cast<long long>(646091557254529188ULL), static_cast<long long>(11393747116974949255ULL), static_cast<long long>(16797249248413342857ULL), static_cast<long long>(15713519023537495495ULL),
    static_cast<long long>(12823504709579858843ULL), static_cast<long long>(4738086532119935073ULL), static_cast<long long>(4429068783387643752ULL), static_cast<long long>(585582692562183870ULL),

    static_cast<long long>(1048280754023674130ULL), static_cast<long long>(6788940719869959076ULL), static_cast<long long>(11670856244972073775ULL), static_cast<long long>(2488756775360218862ULL),
    static_cast<long long>(2061695363573180185ULL), static_cast<long long>(6884655301895085032ULL), static_cast<long long>(3566345954323888697ULL), static_cast<long long>(12784319933059041817ULL),
    static_cast<long long>(4772468691551857254ULL), static_cast<long long>(6864898938209826895ULL), static_cast<long long>(7198730565322227090ULL), static_cast<long long>(2452224231472687253ULL),
    static_cast<long long>(13424792606032445807ULL), static_cast<long long>(10827695224855383989ULL), static_cast<long long>(11016608897122070904ULL), static_cast<long long>(14683280565151378358ULL),

    static_cast<long long>(7077866519618824360ULL), static_cast<long long>(17487079941198422333ULL), static_cast<long long>(3956319990205097495ULL), static_cast<long long>(5804870313319323478ULL),
    static_cast<long long>(8017203611194497730ULL), static_cast<long long>(3310931575584983808ULL), static_cast<long long>(5009341981771541845ULL), static_cast<long long>(11772020174577005930ULL),
    static_cast<long long>(3537640779967351792ULL), static_cast<long long>(6801855569284252424ULL), static_cast<long long>(17687268231192623388ULL), static_cast<long long>(12968358613633237218ULL),
    static_cast<long long>(1429775571144180123ULL), static_cast<long long>(10427377732172208413ULL), static_cast<long long>(12155566091986788996ULL), static_cast<long long>(16465954421598296115ULL),

    static_cast<long long>(12710429690464359999ULL), static_cast<long long>(9547226351541565595ULL), static_cast<long long>(12156624891403410342ULL), static_cast<long long>(2985938688676214686ULL),
    static_cast<long long>(18066917785985010959ULL), static_cast<long long>(5975570403614438776ULL), static_cast<long long>(11541343163022500560ULL), static_cast<long long>(11115388652389704592ULL),
    static_cast<long long>(9499328389494710074ULL), static_cast<long long>(9247163036769651820ULL), static_cast<long long>(3688303938005101774ULL), static_cast<long long>(2210483654336887556ULL),
    static_cast<long long>(15458161910089693228ULL), static_cast<long long>(6558785204455557683ULL), static_cast<long long>(1288373156735958118ULL), static_cast<long long>(18433986059948829624ULL),

    static_cast<long long>(3435082195390932486ULL), static_cast<long long>(16822351800343061990ULL), static_cast<long long>(3120532877336962310ULL), static_cast<long long>(16681785111062885568ULL),
    static_cast<long long>(7835551710041302304ULL), static_cast<long long>(2612798015018627203ULL), static_cast<long long>(15083279177152657491ULL), static_cast<long long>(6591467229462292195ULL),
    static_cast<long long>(10592706450534565444ULL), static_cast<long long>(7438147750787157163ULL), static_cast<long long>(323186165595851698ULL), static_cast<long long>(7444710627467609883ULL),
    static_cast<long long>(8473714411329896576ULL), static_cast<long long>(2782675857700189492ULL), static_cast<long long>(3383567662400128329ULL), static_cast<long long>(3200233909833521327ULL),

    static_cast<long long>(12897601280285604448ULL), static_cast<long long>(3612068790453735040ULL), static_cast<long long>(8324209243736219497ULL), static_cast<long long>(15789570356497723463ULL),
    static_cast<long long>(1083312926512215996ULL), static_cast<long long>(4797349136059339390ULL), static_cast<long long>(5556729349871544986ULL), static_cast<long long>(18266943104929747076ULL),
    static_cast<long long>(1620389818516182276ULL), static_cast<long long>(172225355691600141ULL), static_cast<long long>(3034352936522087096ULL), static_cast<long long>(1266779576738385285ULL),
    static_cast<long long>(3906668377244742888ULL), static_cast<long long>(6961783143042492788ULL), static_cast<long long>(17159706887321247572ULL), static_cast<long long>(4676208075243319061ULL),

    static_cast<long long>(10315634697142985816ULL), static_cast<long long>(13435140047933251189ULL), static_cast<long long>(716076639492622016ULL), static_cast<long long>(13847954035438697558ULL),
    static_cast<long long>(7195811275139178570ULL), static_cast<long long>(10815312636510328870ULL), static_cast<long long>(6214164734784158515ULL), static_cast<long long>(16412194511839921544ULL),
    static_cast<long long>(3862249798930641332ULL), static_cast<long long>(1005482699535576005ULL), static_cast<long long>(4644542796609371301ULL), static_cast<long long>(17600091057367987283ULL),
    static_cast<long long>(4209958422564632034ULL), static_cast<long long>(5419285945389823940ULL), static_cast<long long>(11453701547564354601ULL), static_cast<long long>(9951588026679380114ULL),

    static_cast<long long>(7425168333159839689ULL), static_cast<long long>(8436306210125134906ULL), static_cast<long long>(11216615872596820107ULL), static_cast<long long>(3681345096403933680ULL),
    static_cast<long long>(5770016989916553752ULL), static_cast<long long>(11102855936150871733ULL), static_cast<long long>(11187980892339693935ULL), static_cast<long long>(396336430216428875ULL),
    static_cast<long long>(6384853777489155236ULL), static_cast<long long>(7551613839184151117ULL), static_cast<long long>(16527062023276943109ULL), static_cast<long long>(13429850429024956898ULL),
    static_cast<long long>(9901753960477271766ULL), static_cast<long long>(9731501992702612259ULL), static_cast<long long>(5217575797614661659ULL), static_cast<long long>(10311708346636548706ULL),

    static_cast<long long>(15111747519735330483ULL), static_cast<long long>(4353415295139137513ULL), static_cast<long long>(1845293119018433391ULL), static_cast<long long>(11952006873430493561ULL),
    static_cast<long long>(3531972641585683893ULL), static_cast<long long>(16852246477648409827ULL), static_cast<long long>(15956854822143321380ULL), static_cast<long long>(12314609993579474774ULL),
    static_cast<long long>(16763911684844598963ULL), static_cast<long long>(16392145690385382634ULL), static_cast<long long>(1545507136970403756ULL), static_cast<long long>(17771199061862790062ULL),
    static_cast<long long>(12121348462972638971ULL), static_cast<long long>(12613068545148305776ULL), static_cast<long long>(954203144844315208ULL), static_cast<long long>(1257976447679270605ULL),

    static_cast<long long>(3664184785462160180ULL), static_cast<long long>(2747964788443845091ULL), static_cast<long long>(15895917007470512307ULL), static_cast<long long>(15552935765724302120ULL),
    static_cast<long long>(16366915862261682626ULL), static_cast<long long>(8385468783684865323ULL), static_cast<long long>(10745343827145102946ULL), static_cast<long long>(2485742734157099909ULL),
    static_cast<long long>(916246281077683950ULL), static_cast<long long>(15214206653637466707ULL), static_cast<long long>(12895483149474345798ULL), static_cast<long long>(1079510114301747843ULL),
    static_cast<long long>(10718876134480663664ULL), static_cast<long long>(1259990987526807294ULL), static_cast<long long>(8326303777037206221ULL), static_cast<long long>(14104661172014248293ULL),
};

dCRCTYPE dCombineCRC (dCRCTYPE a, dCRCTYPE b)
{
	return (a << 8) ^ b;
}

// calculate a 32 bit crc of a string
dCRCTYPE dCRC64 (const char* const name, dCRCTYPE crcAcc)
{
	if (name) {
		const int bitshift = (sizeof (dCRCTYPE)<<3) - 8;
		for (int i = 0; name[i]; i ++) {
			char c = name[i];
			dCRCTYPE val = randBits0[((crcAcc >> bitshift) ^ c) & 0xff];
			crcAcc = (crcAcc << 8) ^ val;
		}
	}
	return crcAcc;
}


dCRCTYPE dCRC64 (const void* const buffer, int size, dCRCTYPE crcAcc)
{
	const unsigned char* const ptr = (unsigned char*)buffer;

	const int bitshift = (sizeof (dCRCTYPE)<<3) - 8;
	for (int i = 0; i < size; i ++) {
		char c = ptr[i];
		dCRCTYPE  val = randBits0[((crcAcc >> bitshift) ^ c) & 0xff];
		crcAcc = (crcAcc << 8) ^ val;
	}
	return crcAcc;
}



