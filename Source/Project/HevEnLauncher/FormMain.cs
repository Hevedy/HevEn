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
