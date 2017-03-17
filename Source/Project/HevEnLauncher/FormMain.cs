// ===========================================================================
// 
// HevEn (C) 2017 by Hevedy <https://github.com/Hevedy>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, 
// You can obtain one at https://mozilla.org/MPL/2.0/.
// 
// ---------------------------------------------------------------------------
//  File:			MainForm.cs
//  Description: 	Main form of the launcher
// ---------------------------------------------------------------------------
//  Log:			Source.
//
//
// ===========================================================================

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;

namespace HevEnLauncher {
    public partial class FormMain : Form {
        public FormMain() {
            InitializeComponent();
        }

        bool LaunchEngine() {
            string exeFolder = AppDomain.CurrentDomain.BaseDirectory;

            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = false;
            startInfo.UseShellExecute = false;
            startInfo.FileName = checkBoxDebug.Checked ? exeFolder + "Binaries\\HevEn_Debug.exe" : exeFolder + "Binaries\\HevEn.exe";
            startInfo.WindowStyle = ProcessWindowStyle.Hidden;
            string userFolder = System.Environment.GetFolderPath( Environment.SpecialFolder.MyDocuments ) + "\\My Games\\HevEn";
            startInfo.Arguments = ( userFolder + ( checkBoxLog.Checked ? (" -" + textBoxLog.Text + ".log" ) : "" ) );

            try {
                Process.Start( startInfo );
                return true;
            } catch {
                return false;
                // Error
            }
        }

        private void buttonExec_Click( object sender, EventArgs e ) {
            if ( LaunchEngine() ) {
                System.Windows.Forms.Application.Exit();
            }
        }
    }
}
