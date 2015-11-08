using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TestOliveCSharp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void btnOnChangedFieldV1_Click(object sender, EventArgs e)
        {
            string message;
            var result = TestOlive.onChangedFieldV1(Convert.ToInt32(tbFieldId.Text), tbOldValue.Text, tbNewValue.Text, out message);
            tbResult.Text = Convert.ToString(result);
            tbMessage.Text = message;
        }

        private void btnOnChangedFieldV2_Click(object sender, EventArgs e)
        {
            string message;
            var result = TestOlive.onChangedFieldV2(Convert.ToInt32(tbFieldId.Text), tbOldValue.Text, tbNewValue.Text, out message);
            tbResult.Text = Convert.ToString(result);
            tbMessage.Text = message;
        }
    }
}
