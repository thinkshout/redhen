<?php

/**
 * @file
 * Contains \Drupal\redhen_relation\Form\RelationTypeForm.
 */

namespace Drupal\redhen_relation\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class RelationTypeForm.
 *
 * @package Drupal\redhen_relation\Form
 */
class RelationTypeForm extends EntityForm {
  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    $redhen_relation_type = $this->entity;
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $redhen_relation_type->label(),
      '#description' => $this->t("Label for the Relation type."),
      '#required' => TRUE,
    );

    $form['id'] = array(
      '#type' => 'machine_name',
      '#default_value' => $redhen_relation_type->id(),
      '#machine_name' => array(
        'exists' => '\Drupal\redhen_relation\Entity\RelationType::load',
      ),
      '#disabled' => !$redhen_relation_type->isNew(),
    );

    /* You will need additional form elements for your custom properties. */

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $redhen_relation_type = $this->entity;
    $status = $redhen_relation_type->save();

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Relation type.', [
          '%label' => $redhen_relation_type->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Relation type.', [
          '%label' => $redhen_relation_type->label(),
        ]));
    }
    $form_state->setRedirectUrl($redhen_relation_type->urlInfo('collection'));
  }

}
