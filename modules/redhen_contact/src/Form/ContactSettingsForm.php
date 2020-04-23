<?php

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

    // Must require a valid email to connect Contacts and Drupal Users.
    $connect_users = $form_state->getValue('valid_email') && $form_state->getValue('connect_users');

    // Contacts must be connected to Users if we're going to embed the Contact
    // fields on the User form.
    $embed_on_user_form = $form_state->getValue('connect_users') && $form_state->getValue('embed_on_user_form');

    // Require unique email if we're connecting Contacts to Users.
    $unique_email = $connect_users || $form_state->getValue('unique_email');

    \Drupal::service('config.factory')
      ->getEditable('redhen_contact.settings')
      ->set('valid_email', $form_state->getValue('valid_email'))
      ->set('required_properties', $form_state->getValue('required_properties'))
      ->set('connect_users', $connect_users)
      ->set('embed_on_user_form', $embed_on_user_form)
      ->set('contact_user_form', $form_state->getValue('contact_user_form'))
      ->set('unique_email', $unique_email)
      ->set('alter_username', $form_state->getValue('alter_username'))
      ->set('registration', $form_state->getValue('registration'))
      ->set('registration_type', $form_state->getValue('registration_type'))
      ->set('registration_link', $form_state->getValue('registration_link'))
      ->set('registration_form', $form_state->getValue('registration_form'))
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
    $entity_display_repository = \Drupal::service('entity_display.repository');
    $contact_form_modes = $entity_display_repository->getFormModes('redhen_contact');
    $user_form_options = ['default' => 'Default'];
    foreach ($contact_form_modes as $id => $values) {
      $user_form_options[$id] = $values['label'];
    }
    $form = [
      'valid_email' => [
        '#type' => 'checkbox',
        '#title' => t('Require contacts to have a valid email address'),
        '#description' => t('Controls the contact form validation. Must be enabled to allow Drupal user connections keyed on email. Note that changes will not take effect until cache is rebuilt.'),
        '#default_value' => $config->get('valid_email'),
      ],
      'required_properties' => [
        '#type' => 'checkboxes',
        '#title' => 'Required Names',
        '#description' => $this->t("Select which names to require on Contacts. (Requiring no names can result in unlabeled Contacts.) Note that changes will not take effect until cache is rebuilt."),
        '#options' => [
          'first_name' => $this->t("First Name"),
          'middle_name' => $this->t("Middle Name"),
          'last_name' => $this->t("Last Name"),
        ],
        '#default_value' => $config->get('required_properties'),
      ],
      'connect_users' => [
        '#type' => 'checkbox',
        '#title' => t('Connect users to RedHen contacts'),
        '#description' => t('If checked, RedHen will attempt to connect Drupal users to RedHen contacts by matching email addresses when a contact is updated.'),
        '#default_value' => $config->get('connect_users'),
        '#states' => [
          'visible' => [
            ':input[name="valid_email"]' => ['checked' => TRUE],
          ],
        ],
      ],
      'embed_on_user_form' => [
        '#type' => 'checkbox',
        '#title' => t('Embed RedHen Contact fields on the User edit form'),
        '#description' => t('If checked, the RedHen Contact tab on users will be removed, and the Contact edit fields will instead be attached to the bottom of the User Edit form.'),
        '#default_value' => $config->get('embed_on_user_form'),
        '#states' => [
          'visible' => [
            ':input[name="connect_users"]' => ['checked' => TRUE],
          ],
        ],
      ],
      'contact_user_form' => [
        '#type' => 'select',
        '#options' => $user_form_options,
        '#title' => t('"My Contact" Form'),
        '#description' => t("Select the Contact Form to use on the User's Contact tab or User form."),
        '#default_value' => $config->get('contact_user_form') ? $config->get('contact_user_form') : 'default',
        '#states' => [
          'visible' => [
            ':input[name="connect_users"]' => ['checked' => TRUE],
          ],
        ],
      ],
      'unique_email' => [
        '#type' => 'checkbox',
        '#title' => t('Require Contacts to have a unique email address.'),
        '#description' => t('If checked, all Contacts will have unique email addresses.'),
        '#default_value' => $config->get('unique_email'),
        '#states' => [
          'visible' => [
            ':input[name="connect_users"]' => ['checked' => FALSE],
          ],
        ],
      ],
      'alter_username' => [
        '#type' => 'checkbox',
        '#title' => t('Use contact label as username'),
        '#description' => t("If checked, RedHen will alter the display of the Drupal username to match a linked contact's label."),
        '#default_value' => $config->get('alter_username'),
      ],
      'registration' => [
        '#type' => 'fieldset',
        '#title' => t('User registration'),
        'settings' => [
          'registration' => [
            '#type' => 'checkbox',
            '#options' => [1, 1],
            '#title' => t('Create a contact during user registration'),
            '#default_value' => $config->get('registration'),
          ],
          'registration_type' => [
            '#type' => 'select',
            '#options' => redhen_contact_type_options_list(),
            '#title' => t('Allowed contact type'),
            '#description' => t('Select the contact type to create during registration. (This can be overridden by appending the contact type machine name in the registration url.)'),
            '#default_value' => $config->get('registration_type'),
            '#states' => [
              'visible' => [
                ':input[name="registration"]' => ['checked' => TRUE],
              ],
            ],
          ],
          'registration_form' => [
            '#type' => 'select',
            '#options' => $user_form_options,
            '#title' => t('Registration Contact Form'),
            '#description' => t('Select the Contact Form to embed on the Registration form.'),
            '#default_value' => $config->get('registration_form') ? $config->get('registration_form') : 'default',
            '#states' => [
              'visible' => [
                ':input[name="registration"]' => ['checked' => TRUE],
              ],
            ],
          ],
          'registration_link' => [
            '#type' => 'checkbox',
            '#options' => [1, 1],
            '#title' => t('Link to existing contacts'),
            '#description' => t('If a contact is found with the same email as the new Drupal user, it will be linked to the new account.'),
            '#default_value' => $config->get('registration_link'),
          ],
          'registration_update' => [
            '#type' => 'checkbox',
            '#options' => [1, 1],
            '#title' => t('Update contact fields'),
            '#description' => t('When an existing contact is found and linked to, the submitted field values will overwrite the existing contact field values.'),
            '#default_value' => $config->get('registration_update'),
          ],
        ],
      ],
    ];

    return parent::buildForm($form, $form_state);
  }
}
