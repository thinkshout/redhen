<?php

namespace Drupal\redhen\Form;
use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
/**
 * Configure Redhen settings for this site.
 */
class RedhenAdminSettingsForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormID() {
    return 'redhen_admin_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['redhen.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('redhen.settings');

    $form['redhen_admin_path'] = array(
      '#title' => $this->t('Treat RedHen paths as administrative'),
      '#type' => 'checkbox',
      '#description' => $this->t('This is used by other modules to, for example, use the admin theme on RedHen paths.'),
      '#default_value' => $config->get('redhen_admin_path'),
    );

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('redhen.settings');

    $config
      ->set('redhen_admin_path', $form_state->getValue('redhen_admin_path'))
      ->save();
    parent::submitForm($form, $form_state);
  }
}