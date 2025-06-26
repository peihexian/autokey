#include "actioneditdialog.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QApplication>
#include <QFormLayout>

ActionEditDialog::ActionEditDialog(const KeyAction &action, QWidget *parent)
    : QDialog(parent), m_action(action), m_capturingKey(false) {
    setupUI();
    
    // Set current values
    m_typeComboBox->setCurrentIndex(static_cast<int>(m_action.type));
    m_intervalSpinBox->setValue(m_action.interval);
    m_enabledCheckBox->setChecked(m_action.enabled);
    m_weightSlider->setValue(m_action.weight);
    m_weightLabel->setText(QString::number(m_action.weight));
    m_minIntervalSpinBox->setValue(m_action.minInterval);
    m_maxIntervalSpinBox->setValue(m_action.maxInterval);
    updateKeyDisplay();
    
    setWindowTitle("Edit Action");
    setModal(true);
    resize(300, 200);
}

KeyAction ActionEditDialog::getAction() const {
    return m_action;
}

void ActionEditDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Action settings group
    QGroupBox *settingsGroup = new QGroupBox("Action Settings");
    QFormLayout *formLayout = new QFormLayout(settingsGroup);
    
    // Type selection
    m_typeComboBox = new QComboBox();
    m_typeComboBox->addItem("Keyboard", static_cast<int>(InputType::Keyboard));
    m_typeComboBox->addItem("Mouse Left Click", static_cast<int>(InputType::MouseLeft));
    m_typeComboBox->addItem("Mouse Right Click", static_cast<int>(InputType::MouseRight));
    formLayout->addRow("Type:", m_typeComboBox);
    
    // Key/Button selection
    QHBoxLayout *keyLayout = new QHBoxLayout();
    m_keyDisplay = new QLineEdit();
    m_keyDisplay->setReadOnly(true);
    m_keyButton = new QPushButton("Set Key");
    keyLayout->addWidget(m_keyDisplay);
    keyLayout->addWidget(m_keyButton);
    formLayout->addRow("Key/Button:", keyLayout);
    
    // Interval setting
    m_intervalSpinBox = new QSpinBox();
    m_intervalSpinBox->setRange(100, 60000); // 100ms to 60 seconds
    m_intervalSpinBox->setSuffix(" ms");
    m_intervalSpinBox->setSingleStep(100);
    formLayout->addRow("Interval:", m_intervalSpinBox);
    
    // Enabled checkbox
    m_enabledCheckBox = new QCheckBox("Enabled");
    formLayout->addRow("", m_enabledCheckBox);

    mainLayout->addWidget(settingsGroup);

    // Smart simulation group
    QGroupBox *smartGroup = new QGroupBox("Smart Simulation Settings");
    QFormLayout *smartLayout = new QFormLayout(smartGroup);

    // Weight slider
    QHBoxLayout *weightLayout = new QHBoxLayout();
    m_weightSlider = new QSlider(Qt::Horizontal);
    m_weightSlider->setRange(1, 100);
    m_weightSlider->setValue(50);
    m_weightLabel = new QLabel("50");
    m_weightLabel->setMinimumWidth(30);
    weightLayout->addWidget(m_weightSlider);
    weightLayout->addWidget(m_weightLabel);
    smartLayout->addRow("Weight (1-100):", weightLayout);

    // Min interval
    m_minIntervalSpinBox = new QSpinBox();
    m_minIntervalSpinBox->setRange(50, 10000);
    m_minIntervalSpinBox->setSuffix(" ms");
    m_minIntervalSpinBox->setSingleStep(50);
    m_minIntervalSpinBox->setValue(50);
    smartLayout->addRow("Min Interval:", m_minIntervalSpinBox);

    // Max interval
    m_maxIntervalSpinBox = new QSpinBox();
    m_maxIntervalSpinBox->setRange(100, 60000);
    m_maxIntervalSpinBox->setSuffix(" ms");
    m_maxIntervalSpinBox->setSingleStep(100);
    m_maxIntervalSpinBox->setValue(1000);
    smartLayout->addRow("Max Interval:", m_maxIntervalSpinBox);

    mainLayout->addWidget(smartGroup);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_okButton = new QPushButton("OK");
    m_cancelButton = new QPushButton("Cancel");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ActionEditDialog::onTypeChanged);
    connect(m_keyButton, &QPushButton::clicked, this, &ActionEditDialog::onKeyButtonClicked);
    connect(m_weightSlider, &QSlider::valueChanged, this, &ActionEditDialog::onWeightChanged);
    connect(m_okButton, &QPushButton::clicked, this, &ActionEditDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &ActionEditDialog::reject);
}

void ActionEditDialog::onTypeChanged(int index) {
    InputType type = static_cast<InputType>(m_typeComboBox->itemData(index).toInt());
    m_action.type = type;
    
    // Update key display and enable/disable key button
    if (type == InputType::Keyboard) {
        m_keyButton->setEnabled(true);
        m_keyDisplay->setPlaceholderText("Click 'Set Key' and press a key");
    } else {
        m_keyButton->setEnabled(false);
        m_action.key = 0; // Mouse actions don't need a key
        if (type == InputType::MouseLeft) {
            m_keyDisplay->setText("Left Mouse Button");
        } else {
            m_keyDisplay->setText("Right Mouse Button");
        }
    }
}

void ActionEditDialog::onKeyButtonClicked() {
    if (m_action.type != InputType::Keyboard) {
        return;
    }

    m_capturingKey = true;
    m_keyButton->setText("Press a key...");
    m_keyButton->setEnabled(false);
    m_keyDisplay->setText("Waiting for key press...");

    // Set focus to the dialog to capture key events
    setFocus();
}

void ActionEditDialog::updateKeyDisplay() {
    if (m_action.type == InputType::Keyboard) {
        if (m_action.key != 0) {
            // Convert Windows virtual key code to string representation
            QString keyStr;
            if (m_action.key >= 0x30 && m_action.key <= 0x39) { // VK_0 to VK_9
                keyStr = QString::number(m_action.key - 0x30);
            } else if (m_action.key >= 0x41 && m_action.key <= 0x5A) { // VK_A to VK_Z
                keyStr = QChar('A' + (m_action.key - 0x41));
            } else {
                // Handle special keys
                switch (m_action.key) {
                case 0x20: keyStr = "Space"; break;
                case 0x0D: keyStr = "Enter"; break;
                case 0x09: keyStr = "Tab"; break;
                case 0x10: keyStr = "Shift"; break;
                case 0x11: keyStr = "Ctrl"; break;
                case 0x12: keyStr = "Alt"; break;
                case 0x70: keyStr = "F1"; break;
                case 0x71: keyStr = "F2"; break;
                case 0x72: keyStr = "F3"; break;
                case 0x73: keyStr = "F4"; break;
                case 0x74: keyStr = "F5"; break;
                case 0x75: keyStr = "F6"; break;
                case 0x76: keyStr = "F7"; break;
                case 0x77: keyStr = "F8"; break;
                case 0x78: keyStr = "F9"; break;
                case 0x79: keyStr = "F10"; break;
                case 0x7A: keyStr = "F11"; break;
                case 0x7B: keyStr = "F12"; break;
                default: keyStr = QString("VK_%1").arg(m_action.key, 2, 16, QChar('0')).toUpper();
                }
            }
            m_keyDisplay->setText(keyStr);
        } else {
            m_keyDisplay->setText("");
        }
    } else if (m_action.type == InputType::MouseLeft) {
        m_keyDisplay->setText("Left Mouse Button");
    } else {
        m_keyDisplay->setText("Right Mouse Button");
    }
}

void ActionEditDialog::keyPressEvent(QKeyEvent *event) {
    if (m_capturingKey && m_action.type == InputType::Keyboard) {
        int qtKey = event->key();

        // Handle escape to cancel
        if (qtKey == Qt::Key_Escape) {
            m_capturingKey = false;
            m_keyButton->setText("Set Key");
            m_keyButton->setEnabled(true);
            updateKeyDisplay();
            return;
        }

        // Convert Qt key to Windows virtual key and capture it
        int virtualKey = qtKeyToVirtualKey(qtKey);
        if (virtualKey != 0) {
            m_action.key = virtualKey;
            m_capturingKey = false;
            m_keyButton->setText("Set Key");
            m_keyButton->setEnabled(true);
            updateKeyDisplay();
        }

        event->accept();
        return;
    }

    // Call parent implementation for other keys
    QDialog::keyPressEvent(event);
}

int ActionEditDialog::qtKeyToVirtualKey(int qtKey) {
    // Convert Qt key codes to Windows virtual key codes
    if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9) {
        return 0x30 + (qtKey - Qt::Key_0); // VK_0 to VK_9
    }
    if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
        return 0x41 + (qtKey - Qt::Key_A); // VK_A to VK_Z
    }

    // Handle special keys
    switch (qtKey) {
    case Qt::Key_Space: return 0x20; // VK_SPACE
    case Qt::Key_Return: return 0x0D; // VK_RETURN
    case Qt::Key_Tab: return 0x09; // VK_TAB
    case Qt::Key_Shift: return 0x10; // VK_SHIFT
    case Qt::Key_Control: return 0x11; // VK_CONTROL
    case Qt::Key_Alt: return 0x12; // VK_MENU
    case Qt::Key_F1: return 0x70; // VK_F1
    case Qt::Key_F2: return 0x71; // VK_F2
    case Qt::Key_F3: return 0x72; // VK_F3
    case Qt::Key_F4: return 0x73; // VK_F4
    case Qt::Key_F5: return 0x74; // VK_F5
    case Qt::Key_F6: return 0x75; // VK_F6
    case Qt::Key_F7: return 0x76; // VK_F7
    case Qt::Key_F8: return 0x77; // VK_F8
    case Qt::Key_F9: return 0x78; // VK_F9
    case Qt::Key_F10: return 0x79; // VK_F10
    case Qt::Key_F11: return 0x7A; // VK_F11
    case Qt::Key_F12: return 0x7B; // VK_F12
    default: return 0; // Unsupported key
    }
}

void ActionEditDialog::onWeightChanged(int value) {
    m_weightLabel->setText(QString::number(value));
    m_action.weight = value;
}

void ActionEditDialog::accept() {
    // Update action with current values
    m_action.type = static_cast<InputType>(m_typeComboBox->itemData(m_typeComboBox->currentIndex()).toInt());
    m_action.interval = m_intervalSpinBox->value();
    m_action.enabled = m_enabledCheckBox->isChecked();
    m_action.weight = m_weightSlider->value();
    m_action.minInterval = m_minIntervalSpinBox->value();
    m_action.maxInterval = m_maxIntervalSpinBox->value();

    // Validate
    if (m_action.type == InputType::Keyboard && m_action.key == 0) {
        QMessageBox::warning(this, "Invalid Action", "Please set a key for keyboard actions.");
        return;
    }

    if (m_action.minInterval >= m_action.maxInterval) {
        QMessageBox::warning(this, "Invalid Intervals", "Minimum interval must be less than maximum interval.");
        return;
    }

    QDialog::accept();
}
