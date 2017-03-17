// ===========================================================================
// 
// HevEn (C) 2017 by Hevedy <https://github.com/Hevedy>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, 
// You can obtain one at https://mozilla.org/MPL/2.0/.
// 
// ---------------------------------------------------------------------------
//  File:			Program.cs
//  Description: 	Core of the editor
// ---------------------------------------------------------------------------
//  Log:			Source.
//
//
// ===========================================================================

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace HevEnEditor {
    static class Program {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main() {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault( false );
            Application.Run( new MainForm() );
        }
    }
}
