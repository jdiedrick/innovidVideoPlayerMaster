#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //setup initial ip addresses
    iPadIP = "";
    iPhoneIP = "";
    osxIP = "";
    
    //set framerate
    ofSetFrameRate(24);
    
    //set count
    count = 0;
    
    //set debug and sendosc
    debug = false;
    sendOSC = false;
    
    //setup ui and our first video
    setupUI();
    setupVideo();
    
    //hide cursor
    CGDisplayHideCursor(NULL);
    
    //set gotjson
    gotJSON = false;
    
    //set ss
    ss << "No video information received yet" << endl;
    
    //load json
    loadJSON();
    
    ofRegisterURLNotification(this);
    videosDownloaded = 0;

}

//--------------------------------------------------------------
void ofApp::update(){
    
    player.update();
    
    if(sendOSC){
        
        float p = player.getPosition();
        printf("%f\n",p);
        
        ofxOscMessage m;
        m.setAddress( "/movie/position" );
        m.addFloatArg(p);
        m.addIntArg(debug);
        
        // Broadcast current position information of movie
        if ((count % 100 == 0)) {
            
            osxSender.sendMessage(m); // send slower for macbook
            
        }
        
        iPadSender.sendMessage(m); //send every frame for iphone
        iPhoneSender.sendMessage(m); // send every frame for ipad
        count++;
    }
    

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    player.draw(0,0, 1280, 800);
    
    if(debug){
        drawDebug();
    }
    
    if(drawLoading) {
        ofPushStyle();
        ofSetColor(255, 255, 255, 127);
        ofRect(0,0,ofGetWidth(),ofGetHeight());
        ofPopStyle();
        string vid_dl_string = "Downloaded " + ofToString(videosDownloaded, 2) + " of " + ofToString(numVideosToGet, 2)  + " videos. Please wait!";
        ofDrawBitmapString(vid_dl_string, 0 ,ofGetHeight()/2);
    }
    //cout << "draw loading? : " << drawLoading << endl;


}

# pragma mark - Video and OSC

//--------------------------------------------------------------
void ofApp::setupOSC(){
    //set up osc and counter
    iPadSender.setup(iPadIP, PORT);
    iPhoneSender.setup(iPhoneIP, PORT);
    osxSender.setup(osxIP, PORT);
}

//--------------------------------------------------------------
void ofApp::setupVideo(){
    //load movie and start playing
    player.loadMovie("movies/macbook.mov");
    player.play();
    player.setLoopState(OF_LOOP_NORMAL);

}


#pragma mark - setup ui
//--------------------------------------------------------------
void ofApp::setupUI()
{
    
	gui = new ofxUICanvas(50, 190, 320, 320);
    gui->addWidgetDown(new ofxUILabel("IP Addresses", OFX_UI_FONT_LARGE));
	gui->setWidgetFontSize(OFX_UI_FONT_LARGE);
	gui->addTextInput("iPhone IP", "iPhone IP Address")->setAutoClear(false);
    gui->addTextInput("OS X IP", "Mac Mini IP Address")->setAutoClear(false);
    gui->addTextInput("iPad IP", "iPad IP Address")->setAutoClear(false);
	gui->setWidgetFontSize(OFX_UI_FONT_MEDIUM);
    gui->addToggle("Sync Video", false, 32, 32);
    gui->addButton("Download videos", false, 44, 44);
    //gui->addButton("Update JSON", false, 44, 44);
    vector<string> names;
    names.push_back("Default Video");
    ddl = gui->addDropDownList("SELECT A VIDEO", names);
    ddl->setAutoClose(true);
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    gui->toggleVisible();
    gui->loadSettings("GUI/guiSettings.xml");
     
}


//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
	int kind = e.widget->getKind();
	cout << "got event from: " << name << endl;
	
	if(name == "iPhone IP")
    {
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
        if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            iPhoneIP = textinput->getTextString();
            cout << "ON ENTER: ";
            //            ofUnregisterKeyEvents((testApp*)this);
        }
        else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
        {
            cout << "ON FOCUS: ";
        }
        else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "ON BLUR: ";
            //            ofRegisterKeyEvents(this);
        }
        string output = textinput->getTextString();
        cout << output << endl;
    }
    
    if(name == "OS X IP")
    {
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
        if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            osxIP = textinput->getTextString();
            
            cout << "ON ENTER: ";
            //            ofUnregisterKeyEvents((testApp*)this);
        }
        else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
        {
            cout << "ON FOCUS: ";
        }
        else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "ON BLUR: ";
            //            ofRegisterKeyEvents(this);
        }
        string output = textinput->getTextString();
        cout << output << endl;
    }
    
    if(name == "iPad IP")
    {
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
        if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            
            iPadIP = textinput->getTextString();
            
            cout << "ON ENTER: ";
            //            ofUnregisterKeyEvents((testApp*)this);
        }
        else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
        {
            cout << "ON FOCUS: ";
        }
        else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "ON BLUR: ";
            //            ofRegisterKeyEvents(this);
        }
        string output = textinput->getTextString();
        cout << output << endl;
    }
    
    else if(name == "Sync Video")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        setupOSC();
        sendOSC = toggle->getValue();
    }
    
    else if(name == "Download videos")
    {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        //setupOSC();
        //sendOSC = toggle->getValue();
        if(button->getValue() == 1){
            downloadVideos();
        }
        //cout << name << "\t value: " << button->getValue() << endl;
    }
    
    else if(name == "Update JSON")
    {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        //setupOSC();
        //sendOSC = toggle->getValue();
        if(button->getValue() == 1){
            getJSON();
        }
        //cout << name << "\t value: " << button->getValue() << endl;
    }
    
    else if(name == "SELECT A VIDEO")
    {
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget *> &selected = ddlist->getSelected();
        for(int i = 0; i < selected.size(); i++)
        {
            cout << "SELECTED: " << selected[i]->getName() << endl;
            changeVideo(selected[i]->getName());
        }
    }
}

//--------------------------------------------------------------
void ofApp::drawDebug(){
    //show cursor
    CGDisplayShowCursor(NULL);
    
    //draw some debug shit on screen
    ofDrawBitmapString("frame: " + ofToString(player.getCurrentFrame()) + "/"+ofToString(player.getTotalNumFrames()),50,50);
    ofDrawBitmapString("position: " + ofToString(player.getPosition()),50,70);
    ofDrawBitmapString("iPhone IP: " + iPhoneIP,50,90);
    ofDrawBitmapString("OSX Client IP: "+ osxIP, 50,110);
    ofDrawBitmapString("iPad IP: " + iPadIP ,50,130);
    ofDrawBitmapString("Currently playing: " + player.getMoviePath() ,50,150);
    ofDrawBitmapString("Sending OSC?: " + ofToString(sendOSC) ,50,170);
    ofDrawBitmapString(ss.str(), ofGetWidth()/2, 50); // draw json on right side of screen
}

//--------------------------------------------------------------
void ofApp::updateDDL(){
    cout << "updating ddl" << endl;
    //clear toggles
    ddl->clearToggles();
    //ddl->addToggle("iphone.mov");
    //update our dropdown box with the videos
    for (int i=0; i<response["videos"].size(); i++){
        ddl->addToggle(response["videos"][i]["filename"].asString());
    }
}


#pragma mark - JSON
//--------------------------------------------------------------
void ofApp::getJSON(){
    cout << "Getting JSON..." << endl;
    ss.str(std::string()); // clear string stream
    ss << "Getting JSON..." << endl;
    
    //get json
    std::string url = "http://stormy-badlands-2316.herokuapp.com/data";
    
    if (!response.open(url)) {
		cout  << "Failed to parse JSON\n" << endl;
        ss.str(std::string()); // clear string stream
        ss << "Failed to parse JSON\n" << endl;
	}else{
        gotJSON = true;
        ss.str(std::string()); // clear string stream
        cout << "Got JSON..." << endl;
        
        ofFile file(ofToDataPath("innovid_videos.json") ,ofFile::WriteOnly);
        file << response.getRawString();
        file.close();
        
        //updateJSONDebug();
        string json_file_path = ofToDataPath("innovid_videos.json");
        std::string filePath = json_file_path;
        bool parsingSuccessful = response.open(filePath);
        
        if (parsingSuccessful) {
            cout  << "Parsing successful" << endl;
            
            //updateJSONDebug();
            updateDDL();
            downloadVideos();
        } else {
            cout  << "Failed to parse JSON" << endl;
        }
        
        
        //updateDDL();
        //downloadVideos();
    }
}

//--------------------------------------------------------------
void ofApp::updateJSONDebug(){
    //clear the stringstream
    ss.str(std::string());
    
    //update string stream w json data
    ss<< "number of videos in backend: " << response["videos"].size() << "\n" << endl;
    for (int i=0; i<response["videos"].size(); i++){
        ss << "video " << i+ 1 // +1 for pretty, "non-coder" numbers
        << "\n"
        << "title: "
        << response["videos"][i]["title"].asString()
        << "\n"
        << "filename: "
        << response["videos"][i]["filename"].asString()
        << "\n"
        << "timestamp: "
        <<  response["videos"][i]["timestamp"].asString()
        << "\n"
        << endl;
    }
}

//--------------------------------------------------------------
void ofApp::loadJSON(){
    
    //if json isn't there
    ofFile json;
    string json_file_path = ofToDataPath("innovid_videos.json");
    if ( !json.doesFileExist(json_file_path, false) ) {
        
        //download, save and load json
        //this is done on the ui now by hitting the update json button.
        //could do automatically, but maybe not good if you're not online
        
        cout << "json file doesn't exist, lets try to download it" << endl;
        getJSON();
        
        
    }else if( json.doesFileExist(json_file_path, false) ){
        
        //else, load saved json
        
        cout << "json file exists, lets load it!!!" << endl;
        std::string file = json_file_path;
        
        // Now parse the JSON
        bool parsingSuccessful = response.open(file);
        
        if (parsingSuccessful) {
            updateJSONDebug();
            updateDDL();
        } else {
            cout  << "Failed to parse JSON" << endl;
        }
    }
}


//--------------------------------------------------------------
void ofApp::downloadVideos(){
    
    for (int i=0; i<response["videos"].size(); i++){
        
        ofFile video;
        string video_url = response["videos"][i]["link"].asString();
        string video_filename = response["videos"][i]["filename"].asString();
        string video_final_path = ofToDataPath(video_filename);

        if (!video.doesFileExist(video_final_path)) {
            //[indicator startAnimating];
            drawLoading = true;
            numVideosToGet++;
            fileloader.saveAsync(video_url, video_final_path);
            cout << "downloading video number: " << i  << " url: " << video_url << " final path: " << video_final_path << " num vids to get " << numVideosToGet << endl;
            cout << "draw loading? : " << drawLoading << endl;
        }
        
    }
    
}

//---

void ofApp::urlResponse(ofHttpResponse & httpResponse) {
    if (httpResponse.status==200) {
        cout << "good response" << endl;
        //[indicator stopAnimating];
        videosDownloaded++;
        if(videosDownloaded == numVideosToGet){
            cout << "we have downloaded all of our videos!" << endl;
            drawLoading = false;
            //[indicator stopAnimating];
            // [indicator removeFromSuperview];
        }
        
        cout << "videos downloaded: " << videosDownloaded  << " num videos to get " << numVideosToGet << endl;
        //img.loadImage(response.data);
        //loading = false;
    } else {
        cout << httpResponse.status << " " << httpResponse.error << endl;
        //if (response.status != -1) loading = false;
    }
    
    
}

//--------------------------------------------------------------
void ofApp::changeVideo(string video){
    string video_path = ofToDataPath(video);
    player.stop();
    player.loadMovie(video_path);
    player.play();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='f'){
        ofToggleFullscreen();
    }
    
    if(key=='d'){
        debug = !debug;
        gui->toggleVisible();
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
