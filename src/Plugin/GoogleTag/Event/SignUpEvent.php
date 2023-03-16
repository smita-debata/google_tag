<?php

declare(strict_types=1);

namespace Drupal\google_tag\Plugin\GoogleTag\Event;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\PluginFormInterface;
use Drupal\user\UserInterface;

/**
 * Sign up event plugin.
 *
 * @GoogleTagEvent(
 *   id = "sign_up",
 *   event_name = "sign_up",
 *   label = @Translation("User registration"),
 *   description = @Translation("This event indicates that a user has signed up for an account."),
 *   dependency = "user"
 * )
 *
 * @todo plugin configuration form or something must state this only works if register != REGISTER_ADMINISTRATORS_ONLY
 */
final class SignUpEvent extends EventBase implements PluginFormInterface {

  /**
   * {@inheritDoc}
   */
  public function defaultConfiguration(): array {
    return [
      'method' => 'CMS',
    ];
  }

  /**
   * {@inheritDoc}
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {
    if ($this->configuration['method'] !== UserInterface::REGISTER_ADMINISTRATORS_ONLY) {
      $form['method'] = [
        '#type' => 'textfield',
        '#title' => 'Signup Method',
        '#default_value' => $this->configuration['method'],
        '#description' => $this->t('Sign up method should not be :admin for GA to work.', [':admin' => UserInterface::REGISTER_ADMINISTRATORS_ONLY]),
        '#maxlength' => '254',
      ];
    }
    else {
      $form['markup'] = [
        '#type' => 'markup',
        '#markup' => 'Sign up is admin_only, nothing to be configured here.',
      ];
    }
    return $form;
  }

  /**
   * {@inheritDoc}
   */
  public function validateConfigurationForm(array &$form, FormStateInterface $form_state) {
  }

  /**
   * {@inheritDoc}
   */
  public function submitConfigurationForm(array &$form, FormStateInterface $form_state) {
    $this->configuration['method'] = $form_state->getValue('method');
  }

}
