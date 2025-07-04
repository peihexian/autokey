#ifndef ACTIONEDITDIALOG_H
#define ACTIONEDITDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include "classprofile.h"

class ActionEditDialog : public QDialog {
    Q_OBJECT

public:
    explicit ActionEditDialog(const KeyAction &action, QWidget *parent = nullptr);
    
    KeyAction getAction() const;

public slots:
    void accept() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onTypeChanged(int index);
    void onKeyButtonClicked();
    void onWeightChanged(int value);

private:
    void setupUI();
    void updateKeyDisplay();
    int qtKeyToVirtualKey(int qtKey);

    KeyAction m_action;
    bool m_capturingKey;
    
    QComboBox *m_typeComboBox;
    QLineEdit *m_keyDisplay;
    QPushButton *m_keyButton;
    QSpinBox *m_intervalSpinBox;
    QCheckBox *m_enabledCheckBox;

    // Smart simulation controls
    QSlider *m_weightSlider;
    QLabel *m_weightLabel;
    QSpinBox *m_minIntervalSpinBox;
    QSpinBox *m_maxIntervalSpinBox;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
};

#endif // ACTIONEDITDIALOG_H
