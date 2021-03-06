#include "DataManager.h"

DataManager::DataManager(QDialog* parent) : QDialog(parent), ui(new Ui::DataManager)
{
    ui->setupUi(this);
    ui->noLineEdit->setValidator(new QIntValidator(1, 1000000000, this));
    ui->noLineEdit->setPlaceholderText("10");
}

DataManager::~DataManager()
{

}

void DataManager::on_applyButton_clicked()
{
    if (ui->simRadioButton->isChecked())
    {
        int no = ui->noLineEdit->text().toUInt();
        ui->progressBar->setMinimum(1);
        ui->progressBar->setMaximum(no);

        GenSimFiles simulatio(no);
        for (uint64_t i = 1; i <= no; ++i)
        {
            simulatio.generateFiles(i);
            ui->progressBar->setValue(i);
        }
        this->close();
    }
    else if (ui->statRadioButton->isChecked())
    {
        //GenSystemStat();
    }

    this->close();
}

void DataManager::on_simRadioButton_clicked()
{
    ui->noLineEdit->setEnabled(true);
    ui->progressBar->setEnabled(true);
}

void DataManager::on_statRadioButton_clicked()
{
    ui->noLineEdit->setEnabled(false);
    ui->progressBar->setEnabled(false);
}
