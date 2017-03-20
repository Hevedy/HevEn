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
//  Description: 	Main form of the editor
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

namespace HevEnEditor {
    public partial class MainForm : Form {
        public MainForm() {
            InitializeComponent();
        }

        private void comboBoxMatType_SelectedIndexChanged( object sender, EventArgs e ) {

        }

        private void MainForm_Load( object sender, EventArgs e ) {
            comboBoxMatType.SelectedIndex = 0;
            comboBoxMatClass.SelectedIndex = 0;
            textBoxMatName.Text = "Default";
            checkBoxMatDiffuse.Checked = true;
        }

        private void buttonDiffuse_Click( object sender, EventArgs e ) {

        }

        private void pictureBoxMatDiffuse_DragDrop( object sender, DragEventArgs e ) {

        }

        private void pictureBoxMatNormalMap_DragDrop( object sender, DragEventArgs e ) {

        }

        private void buttonDiffuse_DragDrop( object sender, DragEventArgs e ) {

        }

        private void pictureBoxMatNormalMap_Click( object sender, EventArgs e ) {

        }

        private void buttonMatNormalMap_Click( object sender, EventArgs e ) {

        }

        private void buttonMatNormalMap_DragDrop( object sender, DragEventArgs e ) {

        }

        private void pictureBoxMatSpecMap_Click( object sender, EventArgs e ) {

        }

        private void pictureBoxMatSpecMap_DragDrop( object sender, DragEventArgs e ) {

        }

        private void buttonMatSpecMap_Click( object sender, EventArgs e ) {

        }

        private void buttonMatSpecMap_DragDrop( object sender, DragEventArgs e ) {

        }

        private void pictureBoxMatGlowMap_Click( object sender, EventArgs e ) {

        }

        private void pictureBoxMatGlowMap_DragDrop( object sender, DragEventArgs e ) {

        }

        private void buttonMatGlowMap_Click( object sender, EventArgs e ) {

        }

        private void buttonMatGlowMap_DragDrop( object sender, DragEventArgs e ) {

        }

        private void pictureBoxMatEnvMap_Click( object sender, EventArgs e ) {

        }

        private void pictureBoxMatEnvMap_DragDrop( object sender, DragEventArgs e ) {

        }

        private void buttonMatEnvMap_Click( object sender, EventArgs e ) {

        }

        private void buttonMatEnvMap_DragDrop( object sender, DragEventArgs e ) {

        }
    }
}
