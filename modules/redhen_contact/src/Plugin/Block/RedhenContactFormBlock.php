<?php

namespace Drupal\redhen_contact\Plugin\Block;

use Drupal\Core\Block\BlockBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\redhen_contact\Entity\ContactType;

/**
 * Provides configurable form to add Redhen Contacts.
 *
 * @Block(
 *   id = "redhen_contact_form",
 *   admin_label = @Translation("Redhen Contact Form"),
 * )
 */
class RedhenContactFormBlock extends BlockBase {

  /**
   * Returns form for creating Redhen Contacts.
   */
  public function build() {

    $build = [
      '#markup' => '<p>FOOBAR</p>'
    ];

    return $build;
  }


  /**
   * Form to determine which Redhen Contact fields will appear on the form.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   * @return array
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {

    $form = parent::buildConfigurationForm($form, $form_state);

    $redhen_contact_types = ContactType::loadMultiple();

    $form['redhen_contact_type'] = [
      '#type' => 'select',
      '#title' => $this->t('Select element'),
      '#options' => $redhen_contact_types
    ];

    return $form;
  }

}
