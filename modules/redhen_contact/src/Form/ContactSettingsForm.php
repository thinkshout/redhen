<?php

/**
 * @file
 * Contains \Drupal\redhen_contact\Form\ContactSettingsForm.
 */

namespace Drupal\redhen_contact\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class ContactSettingsForm.
 *
 * @package Drupal\redhen_contact\Form
 *
 * @ingroup redhen_contact
 */
class ContactSettingsForm extends ConfigFormBase {

  /**
   * Returns a unique string identifying the form.
   *
   * @return string
   *   The unique string identifying the form.
   */
  public function getFormId() {
    return 'contact_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['redhen_contact.settings'];
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
      ->getEditable('redhen_contact.settings')
      ->set('valid_email', $form_state->getValue('valid_email'))
      ->set('connect_users', $form_state->getValue('connect_users'))
      ->set('embed_on_user_form', $form_state->getValue('embed_on_user_form'))
      ->set('unique_email', $form_state->getValue('unique_email'))
      ->set('alter_username', $form_state->getValue('alter_username'))
      ->set('registration', $form_state->getValue('registration'))
      ->set('registration_type', $form_state->getValue('registration_type'))
      ->set('registration_link', $form_state->getValue('registration_link'))
      ->set('registration_update', $form_state->getValue('registration_update'))
      ->save();

    parent::submitForm($form, $form_state);
  }

  /**
   * Defines the settings form for Contact entities.
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
    $config = $this->config('redhen_contact.settings');

    $form = array(
      'valid_email' => array(
        '#type' => 'checkbox',
        '#title' => t('Require contacts to have a valid email address'),
        '#description' => t('Controls the contact form validation. Must be true to enable Drupal user connections keyed on email.'),
        '#default_value' => $config->get('valid_email'),
      ),
      'connect_users' => array(
        '#type' => 'checkbox',
        '#title' => t('Connect users to RedHen contacts'),
        '#description' => t('If checked, RedHen will attempt to connect Drupal users to RedHen contacts by matching email addresses when a contact is updated.'),
        '#default_value' => $config->get('connect_users'),
        '#states' => array(
          'enabled' => array(
            ':input[name="valid_email"]' => array('checked' => TRUE),
          ),
          'unchecked' => array(
            ':input[name="valid_email"]' => array('checked' => FALSE),
          ),
        ),
      ),
      'embed_on_user_form' => array(
        '#type' => 'checkbox',
        '#title' => t('Embed RedHen Contact fields on the User edit form'),
        '#description' => t('If checked, the RedHen Contact tab on users will be removed, and the Contact edit fields will instead be attached to the bottom of the User Edit form.'),
        '#default_value' => $config->get('embed_on_user_form'),
        '#states' => array(
          'enabled' => array(
            ':input[name="connect_users"]' => array('checked' => TRUE),
          ),
          'unchecked' => array(
            ':input[name="connect_users"]' => array('checked' => FALSE),
          ),
        ),
      ),
      'unique_email' => array(
        '#type' => 'checkbox',
        '#title' => t('Require Contacts to have a unique email address.'),
        '#description' => t('If checked, all Contacts will have unique email addresses.'),
        '#default_value' => $config->get('unique_email'),
        '#states' => array(
          'visible' => array(
            ':input[name="connect_users"]' => array('checked' => FALSE),
          ),
        ),
      ),
      'alter_username' => array(
        '#type' => 'checkbox',
        '#title' => t('Use contact label as username'),
        '#description' => t("If checked, RedHen will alter the display of the Drupal username to match a linked contact's label."),
        '#default_value' => $config->get('alter_username'),
      ),
      'registration' => array(
        '#type' => 'fieldset',
        '#title' => t('User registration'),
        'settings' => array(
          'registration' => array(
            '#type' => 'checkbox',
            '#options' => array(1, 1),
            '#title' => t('Create a contact during user registration'),
            '#default_value' => $config->get('registration'),
          ),
          'registration_type' => array(
            '#type' => 'select',
            '#options' => redhen_contact_type_options_list(),
            '#title' => t('Allowed contact type'),
            '#description' => t('Select the allowed contact types to create during registration. This can be overridden by appending the contact type machine name in the registration url.'),
            '#default_value' => $config->get('registration_type'),
            '#states' => array(
              'invisible' => array(
                ':input[name="registration"]' => array('checked' => FALSE),
              ),
            ),
          ),
          'registration_link' => array(
            '#type' => 'checkbox',
            '#options' => array(1, 1),
            '#title' => t('Link to existing contacts'),
            '#description' => t('If a contact is found with the same email as the new Drupal user, it will be linked to the new account.'),
            '#default_value' => $config->get('registration_link'),
          ),
          'registration_update' => array(
            '#type' => 'checkbox',
            '#options' => array(1, 1),
            '#title' => t('Update contact fields'),
            '#description' => t('When an existing contact is found and linked to, the submitted field values will overwrite the existing contact field values.'),
            '#default_value' => $config->get('registration_update'),
            '#states' => array(
              'invisible' => array(
                ':input[name="contact_reg_link"]' => array('checked' => FALSE),
              ),
            ),
          ),
        ),
      ),
    );

    return parent::buildForm($form, $form_state);
  }
}
