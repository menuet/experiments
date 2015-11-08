namespace TestOliveCSharp
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.tbFieldId = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.tbOldValue = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.tbNewValue = new System.Windows.Forms.TextBox();
            this.btnOnChangedFieldV1 = new System.Windows.Forms.Button();
            this.btnOnChangedFieldV2 = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.tbResult = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.tbMessage = new System.Windows.Forms.TextBox();
            this.btnOnChangedFieldCppCli = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(99, 75);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(57, 17);
            this.label1.TabIndex = 0;
            this.label1.Text = "Field Id:";
            // 
            // tbFieldId
            // 
            this.tbFieldId.Location = new System.Drawing.Point(179, 72);
            this.tbFieldId.Name = "tbFieldId";
            this.tbFieldId.Size = new System.Drawing.Size(239, 22);
            this.tbFieldId.TabIndex = 1;
            this.tbFieldId.Text = "123";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(99, 103);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(74, 17);
            this.label2.TabIndex = 0;
            this.label2.Text = "Old Value:";
            // 
            // tbOldValue
            // 
            this.tbOldValue.Location = new System.Drawing.Point(179, 100);
            this.tbOldValue.Name = "tbOldValue";
            this.tbOldValue.Size = new System.Drawing.Size(239, 22);
            this.tbOldValue.TabIndex = 1;
            this.tbOldValue.Text = "old";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(99, 131);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(79, 17);
            this.label3.TabIndex = 0;
            this.label3.Text = "New Value:";
            // 
            // tbNewValue
            // 
            this.tbNewValue.Location = new System.Drawing.Point(179, 131);
            this.tbNewValue.Name = "tbNewValue";
            this.tbNewValue.Size = new System.Drawing.Size(239, 22);
            this.tbNewValue.TabIndex = 1;
            this.tbNewValue.Text = "new";
            // 
            // btnOnChangedFieldV1
            // 
            this.btnOnChangedFieldV1.Location = new System.Drawing.Point(103, 183);
            this.btnOnChangedFieldV1.Name = "btnOnChangedFieldV1";
            this.btnOnChangedFieldV1.Size = new System.Drawing.Size(168, 23);
            this.btnOnChangedFieldV1.TabIndex = 2;
            this.btnOnChangedFieldV1.Text = "onChangedFieldV1";
            this.btnOnChangedFieldV1.UseVisualStyleBackColor = true;
            this.btnOnChangedFieldV1.Click += new System.EventHandler(this.btnOnChangedFieldV1_Click);
            // 
            // btnOnChangedFieldV2
            // 
            this.btnOnChangedFieldV2.Location = new System.Drawing.Point(277, 183);
            this.btnOnChangedFieldV2.Name = "btnOnChangedFieldV2";
            this.btnOnChangedFieldV2.Size = new System.Drawing.Size(168, 23);
            this.btnOnChangedFieldV2.TabIndex = 2;
            this.btnOnChangedFieldV2.Text = "onChangedFieldV2";
            this.btnOnChangedFieldV2.UseVisualStyleBackColor = true;
            this.btnOnChangedFieldV2.Click += new System.EventHandler(this.btnOnChangedFieldV2_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(99, 233);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(52, 17);
            this.label4.TabIndex = 0;
            this.label4.Text = "Result:";
            // 
            // tbResult
            // 
            this.tbResult.Location = new System.Drawing.Point(179, 230);
            this.tbResult.Name = "tbResult";
            this.tbResult.Size = new System.Drawing.Size(239, 22);
            this.tbResult.TabIndex = 1;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(99, 261);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(69, 17);
            this.label5.TabIndex = 0;
            this.label5.Text = "Message:";
            // 
            // tbMessage
            // 
            this.tbMessage.Location = new System.Drawing.Point(179, 258);
            this.tbMessage.Name = "tbMessage";
            this.tbMessage.Size = new System.Drawing.Size(431, 22);
            this.tbMessage.TabIndex = 1;
            // 
            // btnOnChangedFieldCppCli
            // 
            this.btnOnChangedFieldCppCli.Location = new System.Drawing.Point(451, 183);
            this.btnOnChangedFieldCppCli.Name = "btnOnChangedFieldCppCli";
            this.btnOnChangedFieldCppCli.Size = new System.Drawing.Size(168, 23);
            this.btnOnChangedFieldCppCli.TabIndex = 2;
            this.btnOnChangedFieldCppCli.Text = "onChangedFieldClppCli";
            this.btnOnChangedFieldCppCli.UseVisualStyleBackColor = true;
            this.btnOnChangedFieldCppCli.Click += new System.EventHandler(this.btnOnChangedFieldCppCli_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(721, 417);
            this.Controls.Add(this.btnOnChangedFieldCppCli);
            this.Controls.Add(this.btnOnChangedFieldV2);
            this.Controls.Add(this.btnOnChangedFieldV1);
            this.Controls.Add(this.tbNewValue);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.tbOldValue);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tbMessage);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.tbResult);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.tbFieldId);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbFieldId;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbOldValue;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tbNewValue;
        private System.Windows.Forms.Button btnOnChangedFieldV1;
        private System.Windows.Forms.Button btnOnChangedFieldV2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbResult;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox tbMessage;
        private System.Windows.Forms.Button btnOnChangedFieldCppCli;
    }
}

