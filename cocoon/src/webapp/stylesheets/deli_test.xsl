<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:param name="deli-capabilities"/>
	<xsl:param name="accept"/>
	<xsl:template match="/">
		<xsl:if test="normalize-space($accept)=''">
			<html>
			<head>
				<title>Test Page for DELI in Cocoon</title>
			</head>
			<body>
				<h1>DELI is switched off</h1>
			</body>
			</html>
		</xsl:if>
		<xsl:if test="contains($accept,'wml') and not(normalize-space($accept)='')">
			<xsl:call-template name="wmldevice"/>
		</xsl:if>
		<xsl:if test="not(contains($accept,'wml')) and not(normalize-space($accept)='')">
			<xsl:call-template name="htmldevice"/>
		</xsl:if>
	</xsl:template>
	<xsl:template name="wmldevice">
		<wml>
			<card id="init" newcontext="true">
				<p>
					<xsl:call-template name="capabilities"/>
				</p>
			</card>
		</wml>
	</xsl:template>
	<xsl:template name="htmldevice">
		<html>
			<head>
				<title>Test Page for DELI in Cocoon</title>
			</head>
			<body>
				<xsl:call-template name="capabilities"/>
			</body>
		</html>
	</xsl:template>
	<xsl:template name="capabilities">
		<xsl:if test="$deli-capabilities/browser/ColorCapable">
ColorCapable: <xsl:value-of select="$deli-capabilities/browser/ColorCapable"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/AudioInputEncoder">
AudioInputEncoder: <xsl:for-each select="$deli-capabilities/browser/AudioInputEncoder/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/DownloadableBrowserApps">
DownloadableBrowserApps: <xsl:for-each select="$deli-capabilities/browser/DownloadableBrowserApps/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/TextInputCapable">
TextInputCapable: <xsl:value-of select="$deli-capabilities/browser/TextInputCapable"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/ImageCapable">
ImageCapable: <xsl:value-of select="$deli-capabilities/browser/ImageCapable"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/CcppAccept">
	CcppAccept:
	<xsl:for-each select="$deli-capabilities/browser/CcppAccept/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/CcppAccept-Charset">
CcppAccept-Charset: <xsl:for-each select="$deli-capabilities/browser/CcppAccept-Charset/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/CcppAccept-Encoding">
CcppAccept-Encoding: <xsl:for-each select="$deli-capabilities/browser/CcppAccept-Encoding/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/CcppAccept-Language">
CcppAccept-Language: <xsl:for-each select="$deli-capabilities/browser/CcppAccept-Language/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/JavaPlatform">
JavaPlatform: <xsl:for-each select="$deli-capabilities/browser/JavaPlatform/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/JVMVersion">
JVMVersion: <xsl:for-each select="$deli-capabilities/browser/JVMVersion/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/XhtmlModules">
XhtmlModules: <xsl:for-each select="$deli-capabilities/browser/XhtmlVersion/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/SupportedPictogramSet">
SupportedPictogramSet: <xsl:for-each select="$deli-capabilities/browser/SupportedPictogramSet/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/OutputCharSet">
OutputCharSet: <xsl:for-each select="$deli-capabilities/browser/OutputCharSet/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/InputCharSet">
InputCharSet: <xsl:for-each select="$deli-capabilities/browser/InputCharSet/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/WmlScriptVersion">
WmlScriptVersion: <xsl:for-each select="$deli-capabilities/browser/WmlScriptVersion/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/WmlVersion">
WmlVersion: <xsl:for-each select="$deli-capabilities/browser/WmlVersion/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/VideoInputEncoder">
VideoInputEncoder: <xsl:for-each select="$deli-capabilities/browser/VideoInputEncoder/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/SupportedBeareres">
SupportedBearers: <xsl:for-each select="$deli-capabilities/browser/SupportedBearers/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/SecuritySupport">
SecuritySupport: <xsl:value-of select="$deli-capabilities/browser/SecuritySupport"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/CurrentBearerService">
CurrentBearerService: <xsl:value-of select="$deli-capabilities/browser/CurrentBearerService"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/BrowserName">
BrowserName: <xsl:value-of select="$deli-capabilities/browser/BrowserName"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/BrowserVersion">
BrowserVersion: <xsl:value-of select="$deli-capabilities/browser/BrowserVersion"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/SupportedBlueToothVersion">
SupportedBluetoothVersion: <xsl:value-of select="$deli-capabilities/browser/SupportedBlueToothVersion"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/SoftwareNumber">
SoftwareNumber: <xsl:value-of select="$deli-capabilities/browser/SoftwareNumber"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/HtmlVersion">
HtmlVersion: <xsl:value-of select="$deli-capabilities/browser/HtmlVersion"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/JavaAppletEnabled">
JavaAppletEnabled: <xsl:value-of select="$deli-capabilities/browser/JavaAppletEnabled"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/JavaScriptEnabled">
JavaScriptEnabled: <xsl:value-of select="$deli-capabilities/browser/JavaScriptEnabled"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/JavaScriptVersion">
JavaScriptVersion: <xsl:value-of select="$deli-capabilities/browser/JavaScriptVersion"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/PreferenceForFrames">
PreferenceForFrames: <xsl:value-of select="$deli-capabilities/browser/PreferenceForFrames"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/TablesCapable">
TablesCapable: <xsl:value-of select="$deli-capabilities/browser/TablesCapable"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/XhtmlVersion">
XhtmlVersion: <xsl:value-of select="$deli-capabilities/browser/XhtmlVersion"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/WapDeviceClass">
WapDeviceClass: <xsl:value-of select="$deli-capabilities/browser/WapDeviceClass"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/WapVersion">
WapVersion: <xsl:value-of select="$deli-capabilities/browser/WapVersion"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/WmlDeckSize">
WmlDeckSize: <xsl:value-of select="$deli-capabilities/browser/WmlDeckSize"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/WmlScriptLibraries">
WmlScriptLibraries: <xsl:for-each select="$deli-capabilities/browser/WmlScriptLibraries/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/WtaiLibraries">
WtaiLibraries: 	<xsl:for-each select="$deli-capabilities/browser/WtaiLibraries/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/WtaiVersion">
WtaiVersion: <xsl:value-of select="$deli-capabilities/browser/WtaiVersion"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/BluetoothProfile">
BluetoothProfile: 	<xsl:for-each select="$deli-capabilities/browser/BluetoothProfile/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/ImageCapable">
ImageCapable: <xsl:value-of select="$deli-capabilities/browser/ImageCapable"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/Keyboard">
Keyboard: <xsl:value-of select="$deli-capabilities/browser/Keyboard"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/CPU">
CPU: <xsl:value-of select="$deli-capabilities/browser/CPU"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/Model">
Model: <xsl:value-of select="$deli-capabilities/browser/Model"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/NumberOfSoftKeys">
NumberOfSoftKeys: <xsl:value-of select="$deli-capabilities/browser/NumberOfSoftKeys"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/PixelAspectRatio">
PixelAspectRatio: <xsl:value-of select="$deli-capabilities/browser/PixelAspectRatio"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/PointingResolution">
PointingResolution: <xsl:value-of select="$deli-capabilities/browser/PointingResolution"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/ScreenSize">
ScreenSize: <xsl:value-of select="$deli-capabilities/browser/ScreenSize"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/ScreenSizeChar">
ScreenSizeChar: <xsl:value-of select="$deli-capabilities/browser/ScreenSizeChar"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/StandardFontProportional">
StandardFontProportional: <xsl:value-of select="$deli-capabilities/browser/StandardFontProportional"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/SoundOutputCapable">
SoundOutputCapable: <xsl:value-of select="$deli-capabilities/browser/SoundOutputCapable"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/TextInputCapable">
TextInputCapable: <xsl:value-of select="$deli-capabilities/browser/TextInputCapable"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/RecipientAppAgent">
RecipientAppAgent: <xsl:value-of select="$deli-capabilities/browser/RecipientAppAgent"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/OSVersion">
OSVersion: <xsl:value-of select="$deli-capabilities/browser/OSVersion"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/MexeSpec">
MexeSpec: <xsl:value-of select="$deli-capabilities/browser/MexeSpec"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/MexeClassmarks">
MexeClassmarks: 	<xsl:for-each select="$deli-capabilities/browser/MexeClassmarks/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/JavaEnabled">
JavaEnabled: <xsl:value-of select="$deli-capabilities/browser/JavaEnabled"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/DownloadableSoftwareSupport">
DownloadableSoftwareSupport: 	<xsl:for-each select="$deli-capabilities/browser/DownloadableSoftwareSupport/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/AcceptDownloadableSoftware">
AcceptDownloadableSoftware: <xsl:value-of select="$deli-capabilities/browser/AcceptDownloadableSoftware"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/VoiceInputCapable">
VoiceInputCapable: <xsl:value-of select="$deli-capabilities/browser/VoiceInputCapable"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/Vendor">
Vendor: <xsl:value-of select="$deli-capabilities/browser/Vendor"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/FramesCapable">
FramesCapable: <xsl:value-of select="$deli-capabilities/browser/FramesCapable"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/BitsPerPixel">
BitsPerPixel: <xsl:value-of select="$deli-capabilities/browser/BitsPerPixel"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/Push-Accept">
Push-Accept: 	<xsl:for-each select="$deli-capabilities/browser/Push-Accept/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/Push-Accept-Charset">
Push-Accept-Charset: 	<xsl:for-each select="$deli-capabilities/browser/Push-Accept-Charset/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/Push-Accept-Encoding">
Push-Accept-Encoding: 	<xsl:for-each select="$deli-capabilities/browser/Push-Accept-Encoding/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/Push-Accept-Language">
Push-Accept-Language: <xsl:for-each select="$deli-capabilities/browser/Push-Accept-Language/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/Push-Accept-AppID">
Push-Accept-AppID: <xsl:for-each select="$deli-capabilities/browser/PushAccept-AppID/li">
				<xsl:value-of select="."/>, 
	</xsl:for-each>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/Push-MsgSize">
Push-MsgSize: <xsl:value-of select="$deli-capabilities/browser/Push-MsgSize"/>
			<br/>
		</xsl:if>
		<xsl:if test="$deli-capabilities/browser/Push-MaxPushReq">
Push-MaxPushReq: <xsl:value-of select="$deli-capabilities/browser/Push-MaxPushReq"/>
			<br/>
		</xsl:if>
	</xsl:template>
</xsl:stylesheet>
