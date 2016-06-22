<?php

namespace Drupal\redhen_connection\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class ConnectionSettingsForm.
 *
 * @package Drupal\redhen_connection\Form
 *
 * @ingroup redhen_connection
 */
class ConnectionSettingsForm extends ConfigFormBase {

  /**
   * Returns a unique string identifying the form.
   *
   * @return string
   *   The unique string identifying the form.
   */
  public function getFormId() {
    return 'connection_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['redhen_connection.settings'];
  }

  /**
   * Form submission handler.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    \Drupal::service('config.factory')
      ->getEditable('redhen_connection.settings')
      ->set('auto_disable_connections', $form_state->getValue('auto_disable_connections'))
      ->save();

    parent::submitForm($form, $form_state);
  }

  /**
   * Defines the settings form for Connection entities.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   *
   * @return array
   *   Form definition array.
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('redhen_connection.settings');

    $form = array(
      'auto_disable_connections' => array(
        '#type' => 'checkbox',
        '#title' => t('Automatically mark connections inactive when either of their endpoints are marked inactive'),
        '#description' => t('When a RedHen Organization or Contact is marked inactive, all of its connections will be marked inactive.'),
        '#default_value' => $config->get('auto_disable_connections'),
      ),
    );

    return parent::buildForm($form, $form_state);
  }
}