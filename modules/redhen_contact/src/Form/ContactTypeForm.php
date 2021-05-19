<?php

namespace Drupal\redhen_contact\Form;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class ContactTypeForm.
 *
 * @package Drupal\redhen_contact\Form
 */
class ContactTypeForm extends EntityForm {
  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    $redhen_contact_type = $this->entity;
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $redhen_contact_type->label(),
      '#description' => $this->t("Label for the contact type."),
      '#required' => TRUE,
    ];

    $form['id'] = [
      '#type' => 'machine_name',
      '#default_value' => $redhen_contact_type->id(),
      '#machine_name' => [
        'exists' => '\Drupal\redhen_contact\Entity\ContactType::load',
      ],
      '#disabled' => !$redhen_contact_type->isNew(),
    ];

    /* You will need additional form elements for your custom properties. */

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $redhen_contact_type = $this->entity;
    $status = $redhen_contact_type->save();

    switch ($status) {
      case SAVED_NEW:
        $this->messenger()->addMessage($this->t('Created the %label Contact type.', [
          '%label' => $redhen_contact_type->label(),
        ]));
        break;

      default:
        $this->messenger()->addMessage($this->t('Saved the %label Contact type.', [
          '%label' => $redhen_contact_type->label(),
        ]));
    }
    $form_state->setRedirectUrl($redhen_contact_type->toUrl('collection'));
  }

}
