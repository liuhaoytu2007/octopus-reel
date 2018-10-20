/*
Octopus-ReEL - Realtime Encephalography Laboratory Network
   Copyright (C) 2007 Barkin Ilhan

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If no:t, see <https://www.gnu.org/licenses/>.

 Contact info:
 E-Mail:  barkin@unrlabs.org
 Website: http://icon.unrlabs.org/staff/barkin/
 Repo:    https://github.com/4e0n/
*/

void parseConfig(QStringList cfgLines) {
 QStringList cfgValidLines,opts,opts2,opts3,
             bufSection,netSection,avgSection,evtSection,
             chnSection,digSection,guiSection,modSection;

 for (int i=0;i<cfgLines.size();i++) { // Isolate valid lines
  if (!(cfgLines[i].at(0)=='#') &&
      cfgLines[i].contains('|')) cfgValidLines.append(cfgLines[i]); }

 // *** CATEGORIZE SECTIONS ***

 initSuccess=true;
 for (int i=0;i<cfgValidLines.size();i++) {
  opts=cfgValidLines[i].split("|");
       if (opts[0].trimmed()=="BUF") bufSection.append(opts[1]);
  else if (opts[0].trimmed()=="NET") netSection.append(opts[1]);
  else if (opts[0].trimmed()=="AVG") avgSection.append(opts[1]);
  else if (opts[0].trimmed()=="EVT") evtSection.append(opts[1]);
  else if (opts[0].trimmed()=="CHN") chnSection.append(opts[1]);
  else if (opts[0].trimmed()=="DIG") digSection.append(opts[1]);
  else if (opts[0].trimmed()=="GUI") guiSection.append(opts[1]);
  else if (opts[0].trimmed()=="MOD") modSection.append(opts[1]);
  else { qDebug("Unknown section in .cfg file!"); initSuccess=false; break; }
 } if (!initSuccess) return;

 // BUF
 if (bufSection.size()>0) {
  for (int i=0;i<bufSection.size();i++) { opts=bufSection[i].split("=");
   if (opts[0].trimmed()=="PAST") { cp.cntPastSize=opts[1].toInt();
    if (!(cp.cntPastSize >= 1000 && cp.cntPastSize <= 20000)) {
     qDebug(".cfg: BUF|PAST not within inclusive (1000,20000) range!");
     initSuccess=false; break;
    }
   }
  }
 } else { cp.cntPastSize=5000; }
 if (!initSuccess) return;

 // NET
 if (netSection.size()>0) {
  for (int i=0;i<netSection.size();i++) { opts=netSection[i].split("=");
   if (opts[0].trimmed()=="STIM") { opts2=opts[1].split(",");
    if (opts2.size()==3) { stimIp=opts2[0].trimmed();
     stimCommPort=opts2[1].toInt(); stimDataPort=opts2[2].toInt();
     opts3=stimIp.split("."); if (opts3.size()==4) {
      // Simple IP validation..
      if ((!(opts3[0].toInt() >= 1  && opts3[0].toInt() < 254)) ||
          (!(opts3[1].toInt() >= 0  && opts3[1].toInt() < 255)) ||
          (!(opts3[2].toInt() >= 0  && opts3[2].toInt() < 255)) ||
          (!(opts3[3].toInt() >= 1  && opts3[3].toInt() < 254)) ||
          // Simple port validation..
          (!(stimCommPort >= 1024 && stimCommPort <= 65535)) ||
          (!(stimDataPort >= 1024 && stimDataPort <= 65535))) {
       qDebug(".cfg: Error in STIM IP and/or port settings!");
       initSuccess=false; break;
      }
     } else {
      qDebug(".cfg: Parse error in STIM IP (v4) Address!");
      initSuccess=false; break;
     }
    } else {
     qDebug(".cfg: Parse error in STIM IP (v4) Address!");
     initSuccess=false; break;
    }
   } else if (opts[0].trimmed()=="ACQ") { opts2=opts[1].split(",");
    if (opts2.size()==3) { acqIp=opts2[0].trimmed();
     acqCommPort=opts2[1].toInt(); acqDataPort=opts2[2].toInt();
     opts3=acqIp.split("."); if (opts3.size()==4) {
      // Simple IP validation..
      if ((!(opts3[0].toInt() >= 1  && opts3[0].toInt() < 254)) ||
          (!(opts3[1].toInt() >= 0  && opts3[1].toInt() < 255)) ||
          (!(opts3[2].toInt() >= 0  && opts3[2].toInt() < 255)) ||
          (!(opts3[3].toInt() >= 1  && opts3[3].toInt() < 254)) ||
          // Simple port validation..
          (!(acqCommPort >= 1024 && acqCommPort <= 65535)) ||
          (!(acqDataPort >= 1024 && acqDataPort <= 65535))) {
       qDebug(".cfg: Error in ACQ IP and/or port settings!");
       initSuccess=false; break;
      }
     } else {
      qDebug(".cfg: Parse error in ACQ IP (v4) Address!");
      initSuccess=false; break;
     }
    }
   } else {
    qDebug(".cfg: Parse error in ACQ IP (v4) Address!");
    initSuccess=false; break;
   }
  }
 } else {
  stimIp="127.0.0.1"; stimCommPort=65000; stimDataPort=65001;
  acqIp="127.0.0.1";  acqCommPort=65002;  stimDataPort=65003;
 } if (!initSuccess) return;

 // Connect to ACQ Server and get crucial info.
 acqCommandSocket->connectToHost(acqIp,acqCommPort);
 acqCommandSocket->waitForConnected();
 QDataStream acqCommandStream(acqCommandSocket);
 csCmd.cmd=CS_ACQ_INFO;
 acqCommandStream.writeRawData((const char*)(&csCmd),sizeof(cs_command));
 acqCommandSocket->flush();
 if (!acqCommandSocket->waitForReadyRead()) {
  qDebug(".cfg: ACQ server did not return crucial info!");
  initSuccess=false; return;
 }
 acqCommandStream.readRawData((char*)(&csCmd),sizeof(cs_command));
 acqCommandSocket->disconnectFromHost();
 if (csCmd.cmd!=CS_ACQ_INFO_RESULT) {
  qDebug(".cfg: ACQ server returned nonsense crucial info!");
  initSuccess=false; return;
 }
 tChns=csCmd.iparam[0]; nChns=(tChns-2)/2; sampleRate=csCmd.iparam[1];
 for (int i=0;i<nChns;i++) { calM.append(1.); calN.append(0.); }
 calA.resize(nChns); calB.resize(nChns);
 calDC.resize(nChns); calSin.resize(nChns);
 acqCurData.resize(tChns+1);

 qDebug(
  ".cfg: ACQ server returned: Chn#=%d, Samplerate=%d",nChns,sampleRate);

 // AVG
 if (avgSection.size()>0) {
  for (int i=0;i<avgSection.size();i++) { opts=avgSection[i].split("=");
   if (opts[0].trimmed()=="INTERVAL") { opts2=opts[1].split(",");
    if (opts2.size()==4) {
     cp.rejBwd=opts2[0].toInt(); cp.avgBwd=opts2[1].toInt();
     cp.avgFwd=opts2[2].toInt(); cp.rejFwd=opts2[3].toInt();
     if ((!(cp.rejBwd >= -1000 && cp.rejBwd <=    0)) ||
         (!(cp.avgBwd >= -1000 && cp.avgBwd <=    0)) ||
         (!(cp.avgFwd >=     0 && cp.avgFwd <= 1000)) ||
         (!(cp.rejFwd >=     0 && cp.rejFwd <= 1000)) ||
         (cp.rejBwd>cp.avgBwd) ||
         (cp.avgBwd>cp.avgFwd) || (cp.avgFwd>cp.rejFwd)) {
      qDebug(".cfg: Logic error in AVG|INTERVAL parameters!");
      initSuccess=false; break;
     } else {
      cp.avgCount=(cp.avgFwd-cp.avgBwd)*sampleRate/1000;
      cp.rejCount=(cp.rejFwd-cp.rejBwd)*sampleRate/1000;
      cp.postRejCount=(cp.rejFwd-cp.avgFwd)*sampleRate/1000;
      cp.bwCount=cp.rejFwd*sampleRate/1000;
     }
    } else {
     qDebug(".cfg: Parse error in AVG|INTERNAL parameters!");
     initSuccess=false; break;
    }
   }
  }
 } else {
  cp.rejBwd=-300; cp.avgBwd=-200; cp.avgFwd=500; cp.rejFwd=600;
  cp.avgCount=700*sampleRate/1000; cp.rejCount=900*sampleRate/1000;
  cp.postRejCount=100*sampleRate/1000; cp.bwCount=600*sampleRate/1000;
 } if (!initSuccess) return;

 if (evtSection.size()>0) {
  for (int i=0;i<evtSection.size();i++) { opts=evtSection[i].split("=");
   if (opts[0].trimmed()=="STIM") { opts2=opts[1].split(",");
    if (opts2.size()==5) {
     if ((!(opts2[0].toInt()>0  && opts2[0].toInt()<128)) ||
         (!(opts2[1].size()>0)) ||
         (!(opts2[2].toInt()>=0 && opts2[2].toInt()<256)) ||
         (!(opts2[3].toInt()>=0 && opts2[3].toInt()<256)) ||
         (!(opts2[4].toInt()>=0 && opts2[4].toInt()<256))) {
      qDebug(".cfg: Syntax/logic error in EVT|STIM parameters!");
      initSuccess=false; break;
     } else {
      dummyEvt=new Event(opts2[0].toInt(),opts2[1].trimmed(),1,
                         QColor(opts2[2].toInt(),
                                opts2[3].toInt(),
                                opts2[4].toInt(),255)); // ..convert to enum!
      acqEvents.append(dummyEvt);
     }
    } else {
     qDebug(".cfg: Parse error in EVT|STIM parameters!");
     initSuccess=false; break;
    }
   } else if (opts[0].trimmed()=="RESP") { opts2=opts[1].split(",");
    if (opts2.size()==5) {
     if ((!(opts2[0].toInt()>0  && opts2[0].toInt()<64)) ||
         (!(opts2[1].size()>0)) ||
         (!(opts2[2].toInt()>=0 && opts2[2].toInt()<256)) ||
         (!(opts2[3].toInt()>=0 && opts2[3].toInt()<256)) ||
         (!(opts2[4].toInt()>=0 && opts2[4].toInt()<256))) {
      qDebug(".cfg: Syntax/logic error in EVT|RESP parameters!");
      initSuccess=false; break;
     } else {
      dummyEvt=new Event(opts2[0].toInt(),opts2[1].trimmed(),2,
                         QColor(opts2[2].toInt(),
                                opts2[3].toInt(),
                                opts2[4].toInt(),255)); // ..convert to enum!
      acqEvents.append(dummyEvt);
     }
    } else {
     qDebug(".cfg: Parse error in EVT|RESP parameters!");
     initSuccess=false; break;
    }
   }
  }
 } else {
  // DEFAULT EVENT SETTINGS (ALL EVENTS OF Octopus-STIM)..
  // (Just a single event pool exists for both stim and resp events)..
  for (int i=0;i<128;i++) {
   dummyString=stimEventNames[i];
   dummyEvt=new Event(i,dummyString,1,QColor(255,0,0,255));
   acqEvents.append(dummyEvt);
  }
  for (int i=0;i<64;i++) {
   dummyString=respEventNames[i];
   dummyEvt=new Event(i,dummyString,2,QColor(255,0,0,255));
   acqEvents.append(dummyEvt);
  }
 } if (!initSuccess) return;

 // CHN
 if (chnSection.size()>0) {
  for (int i=0;i<chnSection.size();i++) { opts=chnSection[i].split("=");
   if (opts[0].trimmed()=="APPEND") { opts2=opts[1].split(",");
    if (opts2.size()==10) {
     opts2[1]=opts2[1].trimmed();
     opts2[4]=opts2[4].trimmed(); opts2[5]=opts2[5].trimmed(); // Trim wspcs
     opts2[6]=opts2[6].trimmed(); opts2[7]=opts2[7].trimmed();
     opts2[8]=opts2[8].trimmed(); opts2[9]=opts2[9].trimmed();
     if ((!(opts2[0].toInt()>0 && opts2[0].toInt()<=nChns)) || // Channel#
         (!(opts2[1].size()>0)) || // Channel name must be at least 1 char..

         (!(opts2[2].toInt()>=0 && opts2[2].toInt()<1000))   || // Rej
         (!(opts2[3].toInt()>=0 && opts2[3].toInt()<=nChns)) || // RejRef

         (!(opts2[4]=="T" || opts2[4]=="t" ||
          opts2[4]=="F" || opts2[4]=="f")) ||
         (!(opts2[5]=="T" || opts2[5]=="t" ||
          opts2[5]=="F" || opts2[5]=="f")) ||
         (!(opts2[6]=="T" || opts2[6]=="t" ||
          opts2[6]=="F" || opts2[6]=="f")) ||
         (!(opts2[7]=="T" || opts2[7]=="t" ||
          opts2[7]=="F" || opts2[7]=="f")) ||

         (!(opts2[8].toFloat()>=0. && opts2[8].toFloat()<=360.)) || // Theta 
         (!(opts2[9].toFloat()>=0. && opts2[9].toFloat()<=360.))){  // Phi
      qDebug(".cfg: Syntax/logic Error in CHN|APPEND parameters!");
      initSuccess=false; break;
     } else { // Set and append new channel..
      dummyChn=new Channel(opts2[0].toInt(),	    // Physical channel
                           opts2[1].trimmed(),	    // Channel Name
                           opts2[2].toInt(),	    // Rejection Level
                           opts2[3].toInt(),	    // Rejection Reference
                           opts2[4],opts2[5],    // Cnt Vis/Rec Flags
                           opts2[6],opts2[7],    // Avg Vis/Rec Flags
                           opts2[8].toFloat(),   // Electrode Cart. Coords
                           opts2[9].toFloat());  // (Theta,Phi)
      dummyChn->pastData.resize(cp.cntPastSize);
      dummyChn->pastFilt.resize(cp.cntPastSize); // Line-noise component
      dummyChn->setEventProfile(acqEvents.size(),cp.avgCount);
      acqChannels.append(dummyChn);
     }
    } else {
     qDebug(".cfg: Parse error in CHN|APPEND parameters!");
     initSuccess=false; break;
    }
   } else if (opts[0].trimmed()=="CALIB") { opts2=opts[1].split(",");
    if (opts2.size()==1) loadCalib_OacFile(opts2[0].trimmed());
    else {
     qDebug(".cfg: Parse error or file not found in CHN|CALIB parameters!");
     qDebug(" Correction disabled/recalibration suggested!..");
    }
   }
  }
 } else {
  // Default channel settings
  QString chnString,noString;
  for (int i=0;i<nChns;i++) {
   chnString="Chn#"+noString.setNum(i);
   dummyChn=new Channel(i,chnString,0,0,"t","t","t","t",0.,0.);
   dummyChn->pastData.resize(cp.cntPastSize);
   dummyChn->pastFilt.resize(cp.cntPastSize);
   acqChannels.append(dummyChn);
  }
 } if (!initSuccess) return;

 // DIG
 if (digSection.size()>0) {
  for (int i=0;i<digSection.size();i++) { opts=digSection[i].split("=");
   if (opts[0].trimmed()=="POLHEMUS") { opts2=opts[1].split(",");
    if (opts2.size()==5) {
     if ((!(opts2[0].toInt()>=0  && opts2[0].toInt()<8)) ||      // ttyS#
         (!(opts2[1].toInt()==9600 ||
            opts2[1].toInt()==19200 ||
            opts2[1].toInt()==38400 ||
            opts2[1].toInt()==57600 ||
            opts2[1].toInt()==115200)) ||                        // BaudRate
         (!(opts2[2].toInt()==7 || opts2[2].toInt()==8)) ||      // Data Bits
         (!(opts2[3].trimmed()=="E" ||
            opts2[3].trimmed()=="O" ||
            opts2[3].trimmed()=="N")) || // Parity
         (!(opts2[4].toInt()==0 || opts2[4].toInt()==1))) {      // Stop Bit
      qDebug(".cfg: Parse/logic error in DIG|POLHEMUS parameters!");
      initSuccess=false; break;
     } else {
      serial.devname="/dev/ttyS"+opts2[0].trimmed();
      switch (opts2[1].toInt()) {
       case   9600: serial.baudrate=B9600;   break;
       case  19200: serial.baudrate=B19200;  break;
       case  38400: serial.baudrate=B38400;  break;
       case  57600: serial.baudrate=B57600;  break;
       case 115200: serial.baudrate=B115200; break;
      }
      switch (opts2[2].toInt()) {
       case 7: serial.databits=CS7; break;
       case 8: serial.databits=CS8; break;
      }
      if (opts2[3].trimmed()=="N") {
       serial.parity=serial.par_on=0;
      } else if (opts2[3].trimmed()=="O") {
       serial.parity=PARODD; serial.par_on=PARENB;
      } else if (opts2[3].trimmed()=="E") {
       serial.parity=0; serial.par_on=PARENB;
      }
      switch (opts2[4].toInt()) {
       default:
       case 0: serial.stopbit=0; break;
       case 1: serial.stopbit=CSTOPB; break;
      }
     }
    } else {
     qDebug(".cfg: Parse error in DIG|POLHEMUS parameters!");
     initSuccess=false; break;
    }
   }
  }
 } else {
  serial.devname="/dev/ttyS0"; serial.baudrate=B115200; serial.databits=CS8;
  serial.parity=serial.par_on=0; serial.stopbit=1;
 } if (!initSuccess) return;

 // GUI
 if (guiSection.size()>0) {
  for (int i=0;i<guiSection.size();i++) { opts=guiSection[i].split("=");
    if (opts[0].trimmed()=="MAIN") { opts2=opts[1].split(",");
    if (opts2.size()==4) {
     guiX=opts2[0].toInt(); guiY=opts2[1].toInt();
     guiWidth=opts2[2].toInt(); guiHeight=opts2[3].toInt();
     if ((!(guiX      >= -2000 && guiX      <= 2000)) ||
         (!(guiY      >= -2000 && guiY      <= 2000)) ||
         (!(guiWidth  >=   640 && guiWidth  <= 2000)) ||
         (!(guiHeight >=   480 && guiHeight <= 2000))) {
      qDebug(".cfg: GUI|MAIN size settings not in appropriate range!");
      initSuccess=false; break;
     }
    } else {
     qDebug(".cfg: Parse error in GUI settings!");
     initSuccess=false; break;
    }
   } else if (opts[0].trimmed()=="HEAD") { opts2=opts[1].split(",");
    if (opts2.size()==4) {
     hwX=opts2[0].toInt(); hwY=opts2[1].toInt();
     hwWidth=opts2[2].toInt(); hwHeight=opts2[3].toInt();
     if ((!(hwX      >= -2000 && hwX      <= 2000)) ||
         (!(hwY      >= -2000 && hwY      <= 2000)) ||
         (!(hwWidth  >=   400 && hwWidth  <= 2000)) ||
         (!(hwHeight >=   300 && hwHeight <= 2000))) {
      qDebug(".cfg: GUI|HEAD size settings not in appropriate range!");
      initSuccess=false; break;
     }
    } else {
     qDebug(".cfg: Parse error in Head Widget settings!");
     initSuccess=false; break;
    }
   }
  }
 } else {
  guiX=60; guiY=60; guiWidth=640; guiHeight=480;
  hwX=160; hwY=160; hwWidth=400; hwHeight=300;
 } if (!initSuccess) return;

 // MOD
 if (modSection.size()>0) {
  for (int i=0;i<modSection.size();i++) { opts=modSection[i].split("=");
   if (opts[0].trimmed()=="GIZMO") { opts2=opts[1].split(",");
    if (opts2.size()==1) {
     if (opts2[0].size()) loadGizmo_OgzFile(opts2[0].trimmed());
     else {
      qDebug(".cfg: MOD|GIZMO filename error!");
      initSuccess=false; break;
     }
    } else {
     qDebug(".cfg: Parse error in MOD|GIZMO parameters!");
     initSuccess=false; break;
    }
   } else if (opts[0].trimmed()=="SCALP") { opts2=opts[1].split(",");
    if (opts2.size()==1) loadScalp_ObjFile(opts2[0].trimmed());
    else {
     qDebug(".cfg: Parse error in MOD|SCALP parameters!");
     initSuccess=false; break;
    }
   } else if (opts[0].trimmed()=="SKULL") { opts2=opts[1].split(",");
    if (opts2.size()==1) loadSkull_ObjFile(opts2[0].trimmed());
    else {
     qDebug(".cfg: Parse error in MOD|SKULL parameters!");
     initSuccess=false; break;
    }
   } else if (opts[0].trimmed()=="BRAIN") { opts2=opts[1].split(",");
    if (opts2.size()==1) loadBrain_ObjFile(opts2[0].trimmed());
    else {
     qDebug(".cfg: Parse error in MOD|BRAIN parameters!");
     initSuccess=false; break;
    }
   }
  }
 } if (!initSuccess) return;
} 
