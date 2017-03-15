namespace HevEnLauncher {
    partial class FormMain {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose( bool disposing ) {
            if ( disposing && ( components != null ) ) {
                components.Dispose();
            }
            base.Dispose( disposing );
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.buttonExec = new System.Windows.Forms.Button();
            this.radioButton86 = new System.Windows.Forms.RadioButton();
            this.radioButton64 = new System.Windows.Forms.RadioButton();
            this.checkBoxDebug = new System.Windows.Forms.CheckBox();
            this.checkBoxLog = new System.Windows.Forms.CheckBox();
            this.textBoxLog = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // buttonExec
            // 
            this.buttonExec.Location = new System.Drawing.Point(12, 12);
            this.buttonExec.Name = "buttonExec";
            this.buttonExec.Size = new System.Drawing.Size(209, 53);
            this.buttonExec.TabIndex = 0;
            this.buttonExec.Text = "Execute";
            this.buttonExec.UseVisualStyleBackColor = true;
            this.buttonExec.Click += new System.EventHandler(this.buttonExec_Click);
            // 
            // radioButton86
            // 
            this.radioButton86.AutoSize = true;
            this.radioButton86.Checked = true;
            this.radioButton86.Location = new System.Drawing.Point(12, 71);
            this.radioButton86.Name = "radioButton86";
            this.radioButton86.Size = new System.Drawing.Size(42, 17);
            this.radioButton86.TabIndex = 1;
            this.radioButton86.TabStop = true;
            this.radioButton86.Text = "x86";
            this.radioButton86.UseVisualStyleBackColor = true;
            // 
            // radioButton64
            // 
            this.radioButton64.AutoSize = true;
            this.radioButton64.Location = new System.Drawing.Point(70, 71);
            this.radioButton64.Name = "radioButton64";
            this.radioButton64.Size = new System.Drawing.Size(42, 17);
            this.radioButton64.TabIndex = 2;
            this.radioButton64.Text = "x64";
            this.radioButton64.UseVisualStyleBackColor = true;
            // 
            // checkBoxDebug
            // 
            this.checkBoxDebug.AutoSize = true;
            this.checkBoxDebug.Checked = true;
            this.checkBoxDebug.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxDebug.Location = new System.Drawing.Point(133, 72);
            this.checkBoxDebug.Name = "checkBoxDebug";
            this.checkBoxDebug.Size = new System.Drawing.Size(88, 17);
            this.checkBoxDebug.TabIndex = 3;
            this.checkBoxDebug.Text = "Debug Mode";
            this.checkBoxDebug.UseVisualStyleBackColor = true;
            // 
            // checkBoxLog
            // 
            this.checkBoxLog.AutoSize = true;
            this.checkBoxLog.Checked = true;
            this.checkBoxLog.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxLog.Location = new System.Drawing.Point(133, 97);
            this.checkBoxLog.Name = "checkBoxLog";
            this.checkBoxLog.Size = new System.Drawing.Size(63, 17);
            this.checkBoxLog.TabIndex = 4;
            this.checkBoxLog.Text = "Log File";
            this.checkBoxLog.UseVisualStyleBackColor = true;
            // 
            // textBoxLog
            // 
            this.textBoxLog.Location = new System.Drawing.Point(12, 95);
            this.textBoxLog.Name = "textBoxLog";
            this.textBoxLog.Size = new System.Drawing.Size(100, 20);
            this.textBoxLog.TabIndex = 5;
            this.textBoxLog.Text = "Log";
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(235, 138);
            this.Controls.Add(this.textBoxLog);
            this.Controls.Add(this.checkBoxLog);
            this.Controls.Add(this.checkBoxDebug);
            this.Controls.Add(this.radioButton64);
            this.Controls.Add(this.radioButton86);
            this.Controls.Add(this.buttonExec);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "HevEn - Launcher";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonExec;
        private System.Windows.Forms.RadioButton radioButton86;
        private System.Windows.Forms.RadioButton radioButton64;
        private System.Windows.Forms.CheckBox checkBoxDebug;
        private System.Windows.Forms.CheckBox checkBoxLog;
        private System.Windows.Forms.TextBox textBoxLog;
    }
}

