<?php

namespace Drupal\redhen_org\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class OrgTypeForm.
 *
 * @package Drupal\redhen_org\Form
 */
class OrgTypeForm extends EntityForm {
  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    $redhen_org_type = $this->entity;
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $redhen_org_type->label(),
      '#description' => $this->t("Label for the org type."),
      '#required' => TRUE,
    );

    $form['id'] = array(
      '#type' => 'machine_name',
      '#default_value' => $redhen_org_type->id(),
      '#machine_name' => array(
        'exists' => '\Drupal\redhen_org\Entity\OrgType::load',
      ),
      '#disabled' => !$redhen_org_type->isNew(),
    );

    /* You will need additional form elements for your custom properties. */

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $redhen_org_type = $this->entity;
    $status = $redhen_org_type->save();

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Org type.', [
          '%label' => $redhen_org_type->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Org type.', [
          '%label' => $redhen_org_type->label(),
        ]));
    }
    $form_state->setRedirectUrl($redhen_org_type->toUrl('collection'));
  }

}
